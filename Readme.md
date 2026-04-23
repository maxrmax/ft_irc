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

Testing checklist (Evaluator guide):
----------------------------------
1. Building: `make` — the target produces the `ircserv` binary.
2. Starting the server: `./ircserv <port> <password>`
    `make run` uses our default port of `6667` and password `start`.
    `make valgrind` uses the port `6668` and password `start`

netcat (nc):
----------------------------------
Commands are usually case sensitive. Our server capitalizes all commands
netcat needs to be told explicitly what it has to do
1. Connect with `nc -C <IP/PC-NAME> <PORT>` - `nc -C 3-G-8 6667`.
2. Authenticate and register: `PASS <password>` -> `NICK <nick>` -> `USER <user> * * :Real Name`.
    example: `pass start` `nick nctest` `user nctest * * nctest` 
3. Join a channel: `JOIN #channel` and verify `PRIVMSG #channel :message` is broadcast to other members.
    `:message` -> `:` tells our server to read until the end of line, which will be `\r\n` -> without `:` it would read until the end of the word
4. Channel operator features: `MODE`, `KICK`, `INVITE`, `TOPIC`.
    `MODE #channel [+|-]o username` to give or take channel operator 
    `MODE #channel [+|-]i` to set or unset a channel invite only 
    `MODE #channel [+|-]k` to set or unset a channel key 
    `MODE #channel [+|-]l LIMIT` to set or unset a user limit for that channel 
    `MODE #channel [+|-]t` to set or unset a channel topic to be changed by operator only
    `KICK #channel username` kick a user from that specific channel (op only)
    `INVITE #channel username` invites an user to join an invite only (+i) channel. Invite is onetime use only.
    `TOPIC #channel :Topic message` changes the channel topic (+t means op only)
5. `PART #ch` disconnect from the given channel
6. `PRIVMSG <#ch|nickname> :message` send a message to a channel or a private message to another user
7. `JARVIS` or `privmsg #ch :@jarvis` to query our running bot. Only the caller will receive the message.
    One invokes directly, the other invokes per channel.
98. Partial command test:
    With `nc`, send a command in parts (use `Ctrl+D` to send fragmented input) to ensure the server aggregates and processes the full line.
    example: joi<CTRL+D>n #<CTRL+D>channel
99. Unexpected disconnects: kill a client and verify server cleans up and continues serving other clients.

irssi:
----------------------------------
irssi takes channel name implicitly by the active tab, `[]` commands are optional, unless a non-active channel is targeted.
irssi sends some commands automatically that our server doesn't support. They can be safely ignored. (MODE #ch b, WHO...)
In irssi you have to sometimes switch views with ctrl+n (next) to change channel, status and private message view. Because of specific irssi behavior, for example dcc, you need to change into the status view with ctrl+n.
1. run `irssi` 
2. Authenticate and register: `/connect <IP/PC-NAME> <port> <pass>`.
    example: `/connect 3-g-8 6667 start`. irssi has local configs to automatically set your username, nickname & realname. 
3. Join a channel: `/JOIN #channel` and verify chatting works by just sending a message.
4. Channel operator features: test `/MODE`, `/KICK`, `/INVITE`, `/TOPIC` as operator and verify errors for non-ops.
    `/MODE [#ch] [+|-]o username` to give or take channel operator 
    `/MODE [#ch] [+|-]i` to set or unset a channel invite only 
    `/MODE [#ch] [+|-]k` to set or unset a channel key 
    `/MODE [#ch] [+|-]l LIMIT` to set or unset a user limit for that channel 
    `/MODE [#ch] [+|-]t` to set or unset a channel topic to be changed by operator only
    `/MODE #ch` to view current applied flags. +o flag is only displayed in irssi by @ in front of a name
    `/KICK [#ch] username` kick a user from that specific channel (op only)
    `/INVITE [#ch] username` invites an user to join an invite only (+i) channel. Invite is onetime use only.
    `/TOPIC [#ch] :Topic message` changes the channel topic (+t means op only) 
5. `/PART [#ch]` disconnect from the active or given channel
6. `/NAMES [#ch]` displays current user list in the given channel (irssi only)
7. `/MSG <nickname> message` send a private message to another user
8. `@jarvis` in a channel to query our running bot. Only the caller will receive the message.
    irssi invokes per channel only.
9. `/DCC send username "filepath"` sending a file transfer request.
    `/dcc get username` to accept a file transfer request
    example: `dcc send irs2 '~/project/rank5/The3Maggots.png'` would send a transfer request to irs2 if the file exists
    example: `dcc get irs1` accept the request and save the file into the home directory `~/The3Maggots.png`
99. Unexpected disconnects: kill a client and verify server cleans up and continues serving other clients.

irc_tester (optional, basically netcat):
----------------------------------
A little stress-test
1. the tester compilation is included in (all)
2. `make tester` and `make tester2` will run the tester
3. `make tester` connects N clients. They run connect.conf then after-connect.conf then repeat the loop.conf
4. `make tester2` connects N clients. Runs the loop once, then disconnects and restarts.

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
