*This project has been created as part of the 42 curriculum by nsloniow, ngoyat, mring*

# ft_irc — Minimal IRC server (ircserv)

Description
-----------
ircserv is a small, single-process IRC server implemented in C++ (compiled with C++17 in this repository). It implements the core functionality required by the 42 subject: client authentication, nick/user registration, channels, private and channel messages, channel operator actions (KICK/INVITE/TOPIC/MODE) and a small built-in bot (`Jarvis`). The project uses a single `poll()` loop and non-blocking sockets to handle multiple clients concurrently.

The server was tested with the reference IRC client `irssi` and with `nc` (netcat). Note: DCC/file transfer support works when using `irssi` only; it does not work with `nc`.

Instructions
------------
Build and run (from project root):

```bash
make
make run          # runs ./ircserv 6667 start
```

Valgrind (helper target):

```bash
make valgrind     # compiles with debug flags and runs valgrind on port 6668
```

How to run the binary
----------------------
Default helper target (when using `make run`):

```bash
./ircserv 6667 start
```

Arguments
---------
- `<port>` — TCP port to listen on (must be >= 1028). Example: `6667`.
- `<password>` — server password required by clients via the `PASS` command.

Example client connections
--------------------------
- Using irssi (recommended / reference client):

	1. Start irssi: `irssi`
	2. Connect: `/connect <ip/pc name> <port> <password>`
    Example: /connect 3-G-10 6667 password

- Using netcat (basic testing):

```bash
nc -C 3-G-10 6667
# Then manually send partial or full IRC commands
```

Quick test (partial command behaviour):

With `nc -C 127.0.0.1 6667` you can send a command in pieces and then press `Ctrl+D` or write `comm^Dand` to flush the line (for the partial-packets test described in the subject): send `PR`, `^D` then `IV` then `MSG #chan :hello\n` and the server will aggregate and process the full command.

Implemented commands / features
-------------------------------
- Authentication: `PASS`, `CAP` (basic capability reply for IRSSI), registration with `NICK` and `USER`.
- Messaging: `PRIVMSG`, `NOTICE` (channel and nick targets).
- Channel handling: `JOIN`, `PART`, `TOPIC`, `NAMES` output, topic queries and replies.
- Channel operator commands: `KICK`, `INVITE`, `MODE` with support for flags: `i` (invite-only), `t` (topic protected), `k` (key), `o` (operator), `l` (limit).
- Connection management: `PING`/`PONG`, `QUIT`.
- Built-in bot: `Jarvis` — available via a `JARVIS` (nc) command or by sending `@jarvis` (irssi) in a channel. Jarvis replies with NOTICE messages (time, help, random facts).

Limitations and notes
---------------------
- DCC / file transfer: only works when using `irssi` (client-side DCC support). File transfer is not supported over `nc`.
- The server is implemented and tested for the features required by the subject. Some numeric replies are simplified or silenced where the RFC allows (for example `NOTICE` intentionally avoids replies per spec).
- The code compiles with C++17 in this repository; the subject requires C++98 but C++17 is campus wide allowed.

Technical details (for evaluators)
---------------------------------
- Single multiplexing loop: The server uses a single `poll()` loop located in [src/network/server/runServer.cpp](src/network/server/runServer.cpp) to handle listening, accepts, reads and writes.
- Non-blocking sockets: `fcntl(fd, F_SETFL, O_NONBLOCK)` is used on the listening socket and accepted client sockets (see [src/network/server/server.cpp](src/network/server/server.cpp) and `acceptClientUser` in [src/network/server/runServer.cpp](src/network/server/runServer.cpp)).
- Only one `poll()` is used for all I/O (reads and writes) as required by the subject. The `poll` helpers are declared in [includes/poll.hpp](includes/poll.hpp).
- Input aggregation: per-client `InputBuffer` collects raw socket reads and provides `hasLine()` / `popLine()` semantics; parsing is implemented in [src/parser/Parser.cpp](src/parser/Parser.cpp).
- Output buffering: per-client `OutputBuffer` stores outgoing messages and `runServer` attempts to send pending bytes when the fd is writable.
- Command dispatch: `CommandDispatcher` maps uppercased command verbs to handler objects ([src/commands/CommandDispatcher.cpp](src/commands/CommandDispatcher.cpp)); each command implementation lives under [src/commands/](src/commands/).

Code layout (important files)
-----------------------------
- Makefile — build targets (`make`, `make run`, `make valgrind`).
- includes/ — public project headers (e.g. `server.hpp`, `Channel.hpp`, `ClientUser.hpp`, `Parser.hpp`).
- src/ — implementation:
	- `src/main.cpp` — program entry and signal setup.
	- `src/network/server/runServer.cpp` — main `poll()` loop and accept/read/write handling.
	- `src/network/server/server.cpp`, `src/network/server/Server_channels.cpp` — server container and channel management.
	- `src/network/client_user/*` — `ClientUser`, `InputBuffer`, `OutputBuffer`.
	- `src/parser/Parser.cpp` — command parser.
	- `src/commands/*` — command handlers (`CmdJoin`, `CmdNick`, `CmdPrivmsg`, `CmdMode`, `CmdKick`, `CmdInvite`, `CmdTopic`, `CmdPass`, `CmdPing`, `CmdQuit`, `CmdUser`, `CmdNotice`, `CmdCap`, `Jarvis` bot).

Security / correctness notes for evaluators
------------------------------------------
- The server sets the listening and accepted sockets to non-blocking mode with `F_SETFL, O_NONBLOCK` as required by the subject.
- `poll()` is called once per loop iteration to inspect readiness. The server handles `POLLIN` (read), `POLLOUT` (write) and error conditions; closed clients are cleaned up and removed from the `pollfd` vector.

Testing checklist (Evaluator guide)
netcat (nc):
----------------------------------
1. Build: `make` — the target produces the `ircserv` binary.
2. Start server: `./ircserv 6667 start` (or `make run`).
3. Connect with `irssi` (recommended) and/or `nc -C 127.0.0.1 6667`.
4. nc: Authenticate and register: send `PASS <password>` then `NICK <nick>` and `USER <user> * * :Real Name`.
5. Join a channel: `JOIN #channel` and verify `PRIVMSG #channel :hello` is broadcast to other members.
6. Channel operator features: test `MODE`, `KICK`, `INVITE`, `TOPIC` as operator and verify errors for non-ops.
7. Partial command test: With `nc`, send a command in parts (use `Ctrl+D` to send fragmented input) to ensure the server aggregates and processes the full line.
8. Unexpected disconnects: kill a client and verify server cleans up and continues serving other clients.

irssi:
----------------------------------
1. Build: `make` — the target produces the `ircserv` binary.
2. Start server: `./ircserv 6667 start` (or `make run`).
3. run `irssi` 
D. Commands are case sensitive but the server capitalizes all commands
4. Authenticate and register: send `/CONNECT <ip> <port> <pass>`.
5. Join a channel: `/JOIN #channel` and verify chatting works by just sending a message.
6. Channel operator features: test `/MODE`, `/KICK`, `/INVITE`, `/TOPIC` as operator and verify errors for non-ops.
7. Unexpected disconnects: kill a client and verify server cleans up and continues serving other clients.

irc_tester (optional, basically netcat):
----------------------------------
1. the tester compilation is included in (all)
2. `make tester` and `make tester2` will run the tester
3. `make tester` connects N clients. Tey run connect.conf then after-connect.conf and then repeat the loop.conf
4. `make tester2` does the same but runs loop once, then disconnects and restarts from the beginning.


##### Unexpected Disconnects:
with valgrind, debug prints will confirm all actions happening within the program.
From construction to destructions, buffer management and the entire poll loop, where necessary.

Classic resources and references
-------------------------------
- RFC 1459 — Internet Relay Chat Protocol (original IRC specification)
- irssi client: https://irssi.org/ (reference client used for testing)
- man pages: `poll(2)`, `socket(2)`, `accept(2)`, `fcntl(2)`, `send(2)`, `recv(2)`
- Valgrind: https://valgrind.org/ (memory debugging)

AI usage disclosure
-------------------
The README content was generated with the assistance of an AI tool to ensure completeness and clarity. The AI was used only to draft and structure the README document; all source code and build artifacts in the repository were written and maintained by the project authors.
AI was used for research and testing code fast and efficiently.
