## Mandatory Part:

##### Program Name:
ircserv

##### Files to Submit:
`*.{h, hpp}`, `*.cpp`, `*.tpp`, `*.ipp`, an optional configuration file

##### Makefile:
`NAME`, `all`, `clean`, `fclean`, `re`

##### Arguments:
- `port` — The listening port
- `password` — The connection password

##### External Functions:
Everything in C++ 98.
`socket`, `close`, `setsockopt`, `getsockname`,
`getprotobyname`, `gethostbyname`, `getaddrinfo`,
`freeaddrinfo`, `bind`, `connect`, `listen`, `accept`,
`htons`, `htonl`, `ntohs`, `ntohl`, `inet_addr`, `inet_ntoa`,
`inet_ntop`, `send`, `recv`, `signal`, `sigaction`,
`sigemptyset`, `sigfillset`, `sigaddset`, `sigdelset`,
`sigismember`, `lseek`, `fstat`, `fcntl`,
`poll` (or equivalent)

##### Extra:
Campus allows to use C++17.
We try to adhere as much as possible to the subject,
but will use C++17 to ease the hassle(s).

---

You are required to develop an IRC server using the C++ 98 standard. (C++17 allowed)
You must not develop an IRC client.
You must not implement server-to-server communication.

Your executable will be run as follows:
```
./ircserv <port> <password>
```
- `port` — The port number on which your IRC server will be listening for incoming IRC connections.
- `password` — The connection password. It will be needed by any IRC client that tries to connect to your server.

> Even though `poll()` is mentioned in the subject and the evaluation scale, you may use any equivalent such as `select()`, `kqueue()`, or `epoll()`.

- The server must be capable of handling multiple clients simultaneously without hanging.
- Forking is prohibited. All I/O operations must be non-blocking.
- Only 1 `poll()` (or equivalent) can be used for handling all these operations (read, write, but also listen, and so forth).

> Because you have to use non-blocking file descriptors, it is possible to use `read`/`recv` or `write`/`send` functions with no `poll()` (or equivalent), and your server wouldn't be blocking. However, it would consume more system resources. Therefore, if you attempt to `read`/`recv` or `write`/`send` in any file descriptor without using `poll()` (or equivalent), your grade will be 0.

- Several IRC clients exist. You have to choose one of them as a reference. Your reference client will be used during the evaluation process.
- Your reference client must be able to connect to your server without encountering any error.
- Communication between client and server has to be done via TCP/IP (v4 or v6).
- Using your reference client with your server must be similar to using it with any official IRC server.

However, you only have to implement the following features:

- You must be able to: authenticate, set a nickname, a username, join a channel, send and receive private messages using your reference client.
- All the messages sent from one client to a channel have to be forwarded to every other client that joined the channel.
- You must have operators and regular users.
- You have to implement the commands that are specific to channel operators:
  - `KICK` — Eject a client from the channel
  - `INVITE` — Invite a client to a channel
  - `TOPIC` — Change or view the channel topic
  - `MODE` — Change the channel's mode:
    - `i` — Set/remove Invite-only channel
    - `t` — Set/remove the restrictions of the TOPIC command to channel operators
    - `k` — Set/remove the channel key (password)
    - `o` — Give/take channel operator privilege
    - `l` — Set/remove the user limit to channel

Of course, you are expected to write a clean code.
Verify every possible error and issue, such as receiving partial data, low bandwidth, etc.

To ensure that your server correctly processes all data sent to it, the following simple test using `nc` can be performed:
```
$> nc -C 127.0.0.1 6667
com^Dman^Dd
$>
```
Use `ctrl+D` to send the command in several parts: `com`, then `man`, then `d\n`.
In order to process a command, you have to first aggregate the received packets in order to rebuild it.

---

## README Requirements

A `README.md` file must be provided at the root of your Git repository. Its purpose is to allow anyone unfamiliar with the project (peers, staff, recruiters, etc.) to quickly understand what the project is about, how to run it, and where to find more information on the topic.

The `README.md` must include at least:

- The very first line must be italicized and read: *This project has been created as part of the 42 curriculum by \<login1>, \<login2>, \<login3>, [...]*
- A **"Description"** section that clearly presents the project, including its goal and a brief overview.
- An **"Instructions"** section containing any relevant information about compilation, installation, and/or execution.
- A **"Resources"** section listing classic references related to the topic (documentation, articles, tutorials, etc.), as well as a description of how AI was used specifying for which tasks and which parts of the project.

Additional sections may be required depending on the project (e.g., usage examples, feature list, technical choices, etc.).
Your README must be written in English.

---

## Bonus Part

Here are additional features you may add to your IRC server to make it resemble an actual IRC server more closely:

- Handle file transfer.
- A bot.

> The bonus part will only be assessed if the mandatory part is PERFECT (100/100). Bonus will be +25/25.

---

## Eval Sheet

### Mandatory Part

#### Basic Checks

- There is a Makefile, the project compiles correctly with the required options, is written in C++, and the executable is called as expected.
- Ask and check how many `poll()` (or equivalent) are present in the code. There must be only one.
- Verify that `poll()` (or equivalent) is called every time before each `accept`, `read`/`recv`, `write`/`send`. After these calls, `errno` should not be used to trigger specific action (e.g. like reading again after `errno == EAGAIN`).
- Verify that each call to `fcntl()` is done as follows: `fcntl(fd, F_SETFL, O_NONBLOCK)`. Any other use of `fcntl()` is forbidden.

> If any of these points is wrong, the evaluation ends now and the final mark is 0.

[YES] \ [NO] (these are buttons)

#### Networking

- The server starts, and listens on all network interfaces on the port given from the command line.
- Using the `nc` tool, you can connect to the server, send commands, and the server answers you back.
- Ask the team what is their reference IRC client.
- Using this IRC client, you can connect to the server.
- The server can handle multiple connections at the same time. The server should not block. It should be able to answer all demands. Do some test with the IRC client and `nc` at the same time.
- Join a channel thanks to the appropriate command. Ensure that all messages from one client on that channel are sent to all other clients that joined the channel.

[YES] \ [NO] (these are buttons)


#### Networking Specials

- Just like in the subject, using `nc`, try to send partial commands. Check that the server answers correctly. With a partial command sent, ensure that other connections still run fine.
- Unexpectedly kill a client. Then check that the server is still operational for the other connections and for any new incoming client.
- Unexpectedly kill a `nc` with just half of a command sent. Check again that the server is not in an odd state or blocked.
- Stop a client (`^Z`) connected on a channel. Then flood the channel using another client. The server should not hang. When the client is live again, all stored commands should be processed normally. Also, check for memory leaks during this operation.

[YES] \ [NO] (these are buttons)


#### Client Commands — Basic

- With both `nc` and the reference IRC client, check that you can authenticate, set a nickname, a username, join a channel.
- Verify that private messages (`PRIVMSG`) are fully functional with different parameters.

[YES] \ [NO] (these are buttons)


#### Channel Operator

- With both nc and the reference IRC client, check that a regular user does not have privileges to do channel operator actions. Then test with an operator. All the channel operation commands should be tested (remove one point for each feature that is not working).

*Rate it from 0 (failed) through 5 (excellent)*
[this is a slider from 0 to 5]

---

### Bonus Part

> Evaluate the bonus part if, and only if, the mandatory part has been entirely and perfectly done, and the error management handles unexpected or bad usage. In case all the mandatory points were not passed during the defense, bonus points must be totally ignored.

#### File Transfer
File transfer works with the reference IRC client.

#### A Small Bot
There's an IRC bot.