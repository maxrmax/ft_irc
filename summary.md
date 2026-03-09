# ft_irc — Project Summary

---

## 1. What is this project?

A single IRC **server** written in C++ (the campus allows C++17 despite the subject saying C++98).  
You **do not** write a client — you connect to the server using the existing `irssi` client (or `nc` for raw testing).

Launch: `./ircserv <port> <password>`  
Example: `./ircserv 6667 secret`

Connect via irssi: `/connect 127.0.0.1 6667`  
Connect via netcat: `nc -C 127.0.0.1 6667`

The reference standard is **RFC 1459** (the original IRC protocol spec):  
https://datatracker.ietf.org/doc/html/rfc1459

---

## 2. What does `readings/bircd/` contain, and do you need it?

`bircd` ("baby ircd") is a **C reference implementation** provided by the 42 school.  
It is written in pure C and uses `select()` instead of `poll()`.

**You do not need to write from it or copy it** — it is purely there so you can:
- See the overall server loop concept in a minimal working form.
- Understand how to structure `accept()` / `select()` / `read()` / `write()` in one loop.

The team's own server does the same thing in C++ with `poll()`.  
`bircd` can also be compiled (`make` in `readings/bircd/`) and run as a reference IRC server to test your client against.

**TL;DR: `bircd` = read-only inspiration. Ignore it as a build artifact.**

---

## 3. Team members (from file headers)

| Login | Role observed |
|---|---|
| `nsloniow` | Lead / majority of core architecture (server loop, server class, most command stubs, buffers) |
| `ngoyat` | Parser, InputBuffer, CommandDispatcher, CmdNick base, command.hpp |
| `mring` | You — `server.hpp` comment cleanup, `CmdPrivmsg.cpp`, `ClientUser.cpp` touch-up |

---

## 4. How the program starts — step by step

```
main()
  check_arguments()          ← validates argc/argv, port must be > 1027 and digits only
  Server irc_server
  irc_server.get_server_ready(port, password)
    socket()                 ← creates a TCP IPv4 socket
    fcntl(O_NONBLOCK)        ← makes the server socket non-blocking
    bind()                   ← attaches socket to 0.0.0.0:<port>
    listen(backlog=1)        ← starts listening  ⚠️ backlog=1 is very low, see bugs
  runServer(irc_server)      ← enters the main poll loop
```

---

## 5. The main poll loop — `runServer.cpp`

This is the heart of the server. One `poll()` call governs everything.

```
runServer()
  creates poll_fd vector         ← starts with just the server fd
  creates poll_clientUser__mapping_via_fd   ← fd → ClientUser map (local to runServer!)
  loop:
    runPoll()
      poll(poll_fd, ..., -1)     ← blocks until at least one fd is ready
      for each fd in poll_fd:
        if POLLIN:
          process_ready_fd()
            if fd == server_fd:
              clientUsers_waiting()
                loop: acceptClientUser()   ← accepts all pending connections
            else:
              receive_message()            ← recv() into ClientUser's InputBuffer
              handleClientInput()          ← parse and dispatch commands
        if POLLOUT:
          process_fd_ready_for_sending()
            sendMsg()                      ← send() from ClientUser's OutputBuffer
```

**Key design choice:** All I/O is non-blocking. `poll()` tells you which fds are ready;  
you only call `recv()`/`send()`/`accept()` on fds that are actually ready.

---

## 6. Class diagram / structure overview

```
Server
├── server_fd            (the listening socket)
├── server_port
├── server_password
├── server_address       (sockaddr_in)
├── CommandDispatcher    (owns all Command* objects)
├── nicknames            (vector<string>)          ← all registered nicks
├── nicknames_history    (unordered_map<nick, [old_nicks]>)
├── nick_clientUser      (unordered_map<nick, ClientUser*>)  ← nick → client ptr
├── _channels            (unordered_map<name, Channel>)
└── _clients             (unordered_map<fd, ClientUser*>)    ← fd → client ptr

ClientUser
├── ClientUser_fd        (their socket fd)
├── InputBuffer          (accumulates raw recv() bytes; extracts \r\n lines)
├── OutputBuffer         (accumulates outgoing text; flushed by sendMsg())
├── nickname / username / realname
├── registered           (bool — true after PASS+NICK+USER accepted)
└── passAccepted         (bool — true after correct PASS)

Channel
├── _name
├── _topic
├── _member_fds          (set<int> — fds of all members)
└── _operator_fd         (int — fd of the first user to join = chanop)

Parser (static methods only)
└── parseLine(raw string) → ParsedCommand { command, params[] }

CommandDispatcher
└── _commands            (map<string, Command*>)
    ├── "CAP"    → CmdCap
    ├── "JOIN"   → CmdJoin
    ├── "NICK"   → CmdNick
    ├── "PASS"   → CmdPass
    ├── "PRIVMSG"→ CmdPrivmsg
    └── "USER"   → CmdUser
    (PART, QUIT, PING/PONG, NOTICE, TOPIC, INVITE, KICK, MODE — commented out/not yet implemented)
```

---

## 7. Data flow for an incoming message

```
Client types "PRIVMSG #general :hello"
  ↓ TCP bytes arrive at kernel buffer
  ↓ poll() signals POLLIN on that fd
  ↓ receive_message()
      recv() → "PRIVMSG #general :hello\r\n" appended to ClientUser.InputBuffer
  ↓ handleClientInput()
      InputBuffer.hasLine()  → true (\r\n found)
      InputBuffer.popLine()  → returns "PRIVMSG #general :hello" (strips \r\n)
      Parser::parseLine()    → ParsedCommand{ "PRIVMSG", ["#general", "hello"] }
      printCommand()         → debug print to stdout
      dispatcher.dispatch()  → calls CmdPrivmsg::execute()
  ↓ CmdPrivmsg::execute()
      check registered, check params
      build prefix ":nick!user@ircserver"
      server.broadcastToChannelExcept("#general", msg, senderFd)
        → appends to OutputBuffer of every member except sender
  ↓ Next poll() loop iteration: POLLOUT fires for each recipient
      sendMsg() → send() from OutputBuffer, erases sent bytes
```

---

## 8. Connection registration flow (as implemented)

RFC 1459 requires: `PASS` → `NICK` → `USER` in that order.

**Current implementation:**
1. `CmdPass::execute()` — checks password. If correct AND client already has a nick set, marks `passAccepted = true` and stores the nick. **⚠️ Bug: only stores nick if nick was already set at PASS time.**
2. `CmdNick::execute()` — validates nickname (letters, digits, special chars; max 9 chars), checks it's not taken, stores it.
3. `CmdUser::execute()` — stores username and realname. **Triggers registration:** if `isReadyToRegister()` (passAccepted + nick + username all set), calls `Nick_ClientUser_mapping()`, sets `registered = true`, sends `001 RPL_WELCOME`.

Only after step 3 does `isRegistered()` return true. Commands like JOIN and PRIVMSG check this.

---

## 9. Commands implemented so far

| Command | File | Status | Notes |
|---|---|---|---|
| `CAP` | `CmdCap.cpp` | ✅ Minimal | Replies `CAP * LS :` (empty capability list). Enough to satisfy irssi handshake. |
| `PASS` | `CmdPass.cpp` | ⚠️ Partial | Password check works, but only links passAccepted if NICK was sent first. See bugs. |
| `NICK` | `CmdNick.cpp` | ⚠️ Partial | Validates and stores nick. Nick-change broadcast to other clients is TODO (commented out). |
| `USER` | `CmdUser.cpp` | ✅ Works | Triggers 001 welcome on successful registration. |
| `JOIN` | `CmdJoin.cpp` | ✅ Works | Multi-channel JOIN (#a,#b), 331/332/353/366 replies, operator assigned to first joiner. |
| `PRIVMSG` | `CmdPrivmsg.cpp` | ✅ Works | Channel and nick targets, correct error codes. |
| `PART` | — | ❌ Missing | Commented out in dispatcher. |
| `QUIT` | — | ❌ Missing | No graceful disconnect. If client drops, server detects read_len==0 and cleans up. |
| `PING/PONG` | — | ❌ Missing | irssi will disconnect if server never sends PING / responds to its PING. |
| `NOTICE` | — | ❌ Missing | |
| `TOPIC` | — | ❌ Missing | Subject mandatory. |
| `INVITE` | — | ❌ Missing | Subject mandatory. |
| `KICK` | — | ❌ Missing | Subject mandatory. |
| `MODE` | — | ❌ Missing | Subject mandatory. Modes: i, t, k, o, l. |

---

## 10. Known bugs and issues (for later review)

### 🔴 Critical

**1. `CmdPass` registration race condition**  
`CmdPass::execute()` only calls `Nicknames_storing()` if the client already has a nick.  
If the client sends `PASS` before `NICK` (which is the standard order), the nick is never stored and `passAccepted` is set but the nick isn't registered in `nicknames`. Later `NickIsAlreadyRegistered()` won't find it. `isReadyToRegister()` in `CmdUser` correctly checks `passAccepted + nick + username`, so registration still works — but `nicknames` vector won't contain the nick.  
→ Fix: `Nicknames_storing()` should be called from `CmdNick` or `CmdUser`, not `CmdPass`.

**2. `isspecial()` is inverted**  
`isSpecial.cpp` returns `false` for all the special characters it checks (- [ ] \\ \` etc.) and `true` otherwise. This is backwards from what `NickIsValid()` expects — it calls `isspecial()` and returns `false` if it returns false. Effect: the special characters listed are **rejected** instead of allowed in nicknames.  
→ Fix: Swap `return false` and `return true` in `isspecial()`.
✅ Fixed

**3. `poll_clientUser__mapping_via_fd` is local to `runServer()`**  
The fd→ClientUser map lives on the stack of `runServer()`. `Server::_clients` is a separate map holding raw pointers into this local map. When `poll_clientUser__mapping_via_fd` reallocates (inserting new clients), all pointers stored in `Server::_clients` and `Server::nick_clientUser` become **dangling**. This is use-after-free waiting to happen.  
→ Fix: Use `std::map` (stable iterators) instead of `std::unordered_map` for `poll_clientUser__mapping_via_fd`, OR move the map into `Server`.

**4. `OutputBuffer::append()` double-appends `\r\n`**  
Every call to `append()` tacks on `\r\n` automatically. But command handlers already include `\r\n` in their message strings. This means every message gets `\r\n\r\n` at the end, confusing IRC clients.  
→ Fix: Either remove the automatic `\r\n` from `OutputBuffer::append()`, or strip it from all message strings in command handlers. (The team probably needs to decide one convention.)

**5. No PING/PONG handling**  
`irssi` sends a `PING` to the server during connection and periodically after. The server ignores it (replies 421 Unknown command). irssi will disconnect after a timeout.  
→ Fix: Add `CmdPing`/`CmdPong` (trivial: reply `:server PONG ircserver :<token>\r\n`).

### 🟡 Medium

**6. `listen()` backlog is 1**  
`server.cpp`: `listen(server_fd, 1)`. Under load (many simultaneous connections), the OS will drop connection attempts beyond 1 in the queue. Increase to at least 10.

**7. `NickIsAlreadyRegistered` uses a linear search on a `vector`**  
`std::find()` on a `vector<string>` is O(n). With many clients it'll slow down. `nick_clientUser` (an `unordered_map`) already exists and is the right structure. Use that for the collision check instead.

**8. Nick-change broadcast not implemented**  
`CmdNick.cpp` has a big TODO comment. If a registered client changes their nick, no other clients are notified. `nicknames` vector also keeps the old nick forever (no removal on change).

**9. `Server::unregisterClientFd()` is declared in `server.hpp` but not defined anywhere**  
It's called in `receive_message()` when a client disconnects (read_len==0). This will fail to link or crash.  
→ Check if this causes a compile error; if not, it's a linker issue waiting to surface. Add an implementation.

**10. `Channel` stores only one operator (`_operator_fd`)**  
The subject requires `MODE +o` to give/take operator privilege to any user, which means multiple operators must be supported. The current design (single int) doesn't allow that.  
→ Fix: Change `_operator_fd` to a `std::set<int>`.

**11. Headers are stale / out of sync with implementations**  
`ircserv.hpp` still declares old `runServer.cpp` function signatures (e.g. `process_ready_fd(Server&, vector<pollfd>&, int)` with 3 args) but the actual implementations now take 4 args (`+unordered_map` param). This may cause warnings or subtle issues.

### 🟢 Minor / Style

**12. `nick_name` and `user_name` fields in `ClientUser.hpp` are unused**  
The header declares both `nick_name`/`user_name` (old style) AND `nickname`/`username` (new style). Only the latter is used. Remove `nick_name` and `user_name` to avoid confusion.

**13. `message.cpp` is empty**  
`src/network/message.cpp` exists but has no content. Either use it or remove it.

**14. Debug prints everywhere**  
`printCommand()` is called for every single incoming command. This will spam the server terminal. Comment it out before evaluation.

**15. `OutputBuffer::popLine()` is declared but not implemented**  
The `.hpp` declares `popLine()` but the `.cpp` only has `append()` and `get_buffer()`. Not currently called anywhere so it won't fail yet, but it's dead code.

---

## 11. What still needs to be implemented (mandatory for 100%)

From `research.md` and the eval sheet, nothing in the list below passes evaluation yet:

- [ ] `PART` — leave a channel
- [ ] `QUIT` — graceful disconnect (also handle abrupt disconnect = close + QUIT broadcast to channels)
- [ ] `PING` / `PONG` — keepalive (irssi needs this)
- [ ] `NOTICE` — like PRIVMSG but no auto-replies
- [ ] `TOPIC` — view/set channel topic (respect `+t` mode)
- [ ] `INVITE` — invite user to channel (required for `+i` channels)
- [ ] `KICK` — chanop kicks a user
- [ ] `MODE` — channel modes `i`, `t`, `k`, `o`, `l`
- [ ] Welcome sequence after registration: `001`, `002`, `003`, `004`, MOTD (`375`, `372`, `376` or `422`), LUSER (`251`, `252`, `254`, `255`)
- [ ] Broadcast nick changes to other connected clients
- [ ] `ERR_NOTREGISTERED (451)` for any command before registration (only JOIN and PRIVMSG currently check this)
- [ ] `MODE` for users (invisible `+i`, etc.) — lower priority

---

## 12. Program flow diagram (text version)

```
startup
  └─ main()
       ├─ check_arguments()
       ├─ Server::get_server_ready()   [socket → fcntl → bind → listen]
       └─ runServer()
            ├─ init poll_fd[]  [server_fd with POLLIN|POLLOUT]
            ├─ init poll_clientUser__mapping_via_fd  {}
            └─ loop forever
                 └─ runPoll()
                      ├─ poll(poll_fd, n, -1)   ← BLOCKS until something is ready
                      └─ for fd in poll_fd:
                           ├─ POLLIN?
                           │   ├─ fd == server_fd?
                           │   │   └─ clientUsers_waiting()
                           │   │        └─ loop: acceptClientUser()
                           │   │             ├─ accept()            ← new fd
                           │   │             ├─ fcntl(O_NONBLOCK)
                           │   │             ├─ poll_fd.push_back() ← add to poll list
                           │   │             ├─ ClientUser(fd)      ← create object
                           │   │             └─ Server::registerClientFd()
                           │   └─ fd == client_fd?
                           │       ├─ receive_message()
                           │       │   ├─ recv() → raw bytes
                           │       │   ├─ append to InputBuffer
                           │       │   └─ if read==0: disconnect + erase from maps
                           │       └─ handleClientInput()
                           │           └─ while InputBuffer.hasLine():
                           │               ├─ popLine()        ← strip \r\n
                           │               ├─ Parser::parseLine()
                           │               └─ dispatcher.dispatch()
                           │                   └─ CmdXxx::execute()
                           │                       └─ appends to OutputBuffer(s)
                           └─ POLLOUT?
                               └─ fd != server_fd?
                                   └─ sendMsg()
                                       ├─ send() from OutputBuffer
                                       └─ erase sent bytes from buffer
```

---

## 13. File index (quick reference)

| File | Who | What |
|---|---|---|
| `main.cpp` | nsloniow | Entry point, arg check, server init |
| `includes/ircserv.hpp` | nsloniow | Master include, global constants, forward declarations |
| `includes/server.hpp` | nsloniow / mring | Server class declaration |
| `src/network/server/server.cpp` | nsloniow | Server constructor, socket setup, nick/channel helpers |
| `src/network/server/Server_channels.cpp` | nsloniow/mring | Channel CRUD, broadcast, client fd registry |
| `src/network/server/runServer.cpp` | nsloniow | poll loop, accept, recv, send |
| `includes/ClientUser.hpp` | nsloniow | ClientUser class declaration |
| `src/network/client_user/ClientUser.cpp` | nsloniow / mring | ClientUser implementation |
| `src/network/client_user/InputBuffer.cpp` | ngoyat | Recv buffer, CRLF line extraction |
| `src/network/client_user/OuputBuffer.cpp` | nsloniow | Send buffer (note: typo in filename) |
| `includes/Channel.hpp` | nsloniow | Channel class declaration |
| `src/network/channel/Channel.cpp` | nsloniow | Channel implementation |
| `includes/command.hpp` | ngoyat | Abstract `Command` base class |
| `includes/commandDispatcher.hpp` | ngoyat | Dispatcher declaration |
| `src/commands/CommandDispatcher.cpp` | ngoyat | Dispatcher: maps string → Command*, calls execute() |
| `src/commands/handleClientInput.cpp` | nsloniow | Drains InputBuffer, parses, dispatches |
| `includes/Parser.hpp` | ngoyat | Parser declaration, ParsedCommand struct |
| `src/parser/Parser.cpp` | ngoyat | IRC line parser |
| `src/commands/CmdCap.cpp` | nsloniow | CAP LS reply |
| `src/commands/CmdPass.cpp` | nsloniow | PASS auth |
| `src/commands/CmdNick.cpp` | ngoyat | NICK validation and storage |
| `src/commands/CmdUser.cpp` | ngoyat / nsloniow | USER → triggers 001 welcome |
| `src/commands/CmdJoin.cpp` | nsloniow | JOIN with multi-channel, 331/332/353/366 |
| `src/commands/CmdPrivmsg.cpp` | nsloniow / mring | PRIVMSG to channel or nick |
| `src/checker/isDigit.cpp` | nsloniow | Port number validation |
| `src/checker/isSpecial.cpp` | nsloniow | Nick special char check (**inverted logic bug**) |
| `src/network/message.cpp` | — | Empty file |
| `readings/bircd/` | 42 school | C reference server, read-only |
| `readings/subject_stuff.txt` | team | Early notes on subject |
| `research.md` | team (mring?) | RFC 1459 analysis, what's missing |
| `subject requirements.md` | mring | Subject spec + eval sheet |

---

## 14. How to build and test

```bash
# Build
make

# Run server
./ircserv 6667 mypassword

# Test with nc (raw IRC)
nc -C 127.0.0.1 6667
# Then type:
PASS mypassword
NICK testbot
USER testbot 0 * :Test User

# Test with irssi
irssi
/connect 127.0.0.1 6667 mypassword

# Partial send test (Ctrl+D between segments)
nc -C 127.0.0.1 6667
PASS mypas<Ctrl+D>sword<Enter>
```

---

## 15. Suggested next steps (priority order)

1. **Fix `isspecial()` bug** — quickest fix, breaks nick validation right now.
2. **Add `CmdPing`/`CmdPong`** — irssi will not stay connected without this.
3. **Fix `OutputBuffer::append()` double `\r\n`** — clients will see garbled output.
4. **Fix pointer invalidation** — use `std::map` or move the client map into `Server`.
5. **Add `PART` and `QUIT`** — needed for basic channel usage.
6. **Add missing mandatory commands** — `TOPIC`, `INVITE`, `KICK`, `MODE`.
7. **Add full welcome sequence** (001–004, MOTD, LUSER) — irssi expects these.
8. **Make `Channel` support multiple operators** — needed for `MODE +o`.
9. **Clean up debug prints** before evaluation.
