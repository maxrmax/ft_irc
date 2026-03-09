Research about:

server loop
client loop?

`socket`
`close`
`setsockopt`
`getsockname`
`getprotobyname`
`gethostbyname`
`getaddrinfo`
`freeaddrinfo`
`bind`
`connect`
`listen`
`accept`
`htons`
`htonl`
`ntohs`
`ntohl`
`inet_addr`
`inet_ntoa`
`inet_ntop`
`send`
`recv`
`signal`
`sigaction`
`sigemptyset`
`sigfillset`
`sigaddset`
`sigdelset`
`sigismember`
`lseek`
`fstat`
`fcntl`
`poll` (or equivalent)


IRC:
`KICK`
`INVITE`
`TOPIC`
`MODE` - `i` `t` `k` `o` `l`

Bonus:

File transfer
bot

---

## RFC 1459 — What we still need to implement

### Already done (dispatcher confirmed)
- `CAP` — capability negotiation
- `PASS` — connection password (§4.1.1)
- `NICK` — set/change nickname (§4.1.2)
- `USER` — register username/realname (§4.1.3)
- `JOIN` — join a channel (§4.2.1)
- `PRIVMSG` — send a message to user or channel (§4.4.1)

---

### Missing mandatory commands

#### `PART` — Leave a channel (§4.2.2)
- Syntax: `PART <channel>{,<channel>}`
- Remove the sending client from the listed channels
- Broadcast the PART to all channel members
- Numerics: `ERR_NEEDMOREPARAMS (461)`, `ERR_NOSUCHCHANNEL (403)`, `ERR_NOTONCHANNEL (442)`

#### `QUIT` — Disconnect gracefully (§4.1.6)
- Syntax: `QUIT [<quit message>]`
- Server closes the connection; broadcasts a QUIT to all channels the user was in
- If connection drops without QUIT (EOF), server generates a QUIT on their behalf
- Numerics: none

#### `PING` / `PONG` — Connection liveness (§4.6.2 / §4.6.3)
- Syntax: `PING <server>` / `PONG <daemon>`
- Server sends PING to clients it hasn't heard from; client must reply PONG immediately
- If no PONG in time → close connection (see §8.4)
- Numerics: `ERR_NOORIGIN (409)`, `ERR_NOSUCHSERVER (402)`

#### `NOTICE` — Send a notice (§4.4.2)
- Syntax: `NOTICE <target> <text>`
- Like PRIVMSG but **no automatic replies must ever be generated** in response
- Server must not send error replies back to the client on receipt of a NOTICE

#### `TOPIC` — View or change channel topic (§4.2.4)
- Syntax: `TOPIC <channel> [<topic>]`
- With no topic param → return current topic (`RPL_TOPIC 332` or `RPL_NOTOPIC 331`)
- With topic param → change it if mode `+t` is not set, or if sender is a chanop
- Numerics: `ERR_NEEDMOREPARAMS (461)`, `ERR_NOTONCHANNEL (442)`, `ERR_CHANOPRIVSNEEDED (482)`, `RPL_NOTOPIC (331)`, `RPL_TOPIC (332)`

#### `INVITE` — Invite a user to a channel (§4.2.7)
- Syntax: `INVITE <nickname> <channel>`
- Sender must be on the channel; if channel is `+i`, sender must be a chanop
- Server sends `RPL_INVITING` to the inviter and a notice to the invitee
- Numerics: `ERR_NEEDMOREPARAMS (461)`, `ERR_NOSUCHNICK (401)`, `ERR_NOTONCHANNEL (442)`, `ERR_USERONCHANNEL (443)`, `ERR_CHANOPRIVSNEEDED (482)`, `RPL_INVITING (341)`

#### `KICK` — Forcibly remove a user from a channel (§4.2.8)
- Syntax: `KICK <channel> <user> [<comment>]`
- Only a channel operator may kick; server validates sender is chanop before acting
- Broadcast KICK to all channel members, then remove the victim
- Numerics: `ERR_NEEDMOREPARAMS (461)`, `ERR_NOSUCHCHANNEL (403)`, `ERR_BADCHANMASK (476)`, `ERR_CHANOPRIVSNEEDED (482)`, `ERR_NOTONCHANNEL (442)`

#### `MODE` — Channel and user mode changes (§4.2.3)
- Syntax: `MODE <channel|nick> {[+|-]<modes>} [<params>]`
- Parse the full mode string before applying changes
- Only chanops can change channel modes

**Channel modes to implement (mandatory):**

| Mode | Description | Has param? |
|------|-------------|------------|
| `+i` / `-i` | Invite-only — only invited users can join | No |
| `+t` / `-t` | Topic locked — only chanops can change topic | No |
| `+k` / `-k` | Channel key (password) — `JOIN` must supply it | Yes (`+k <key>`, `-k *`) |
| `+o` / `-o` | Give/take chanop privilege to a user | Yes (nick) |
| `+l` / `-l` | User limit — max number of members | Yes (`+l <n>`, `-l` no param) |

**User modes to know about (§4.2.3.2):** `i` (invisible), `o` (operator) — users cannot grant themselves `+o`

- Numerics: `ERR_NEEDMOREPARAMS (461)`, `ERR_CHANOPRIVSNEEDED (482)`, `ERR_NOSUCHNICK (401)`, `ERR_NOTONCHANNEL (442)`, `ERR_KEYSET (467)`, `ERR_UNKNOWNMODE (472)`, `ERR_BADCHANNELKEY (475)`, `ERR_CHANNELISFULL (471)`, `ERR_INVITEONLYCHAN (473)`, `RPL_CHANNELMODEIS (324)`, `RPL_BANLIST (367)`, `RPL_ENDOFBANLIST (368)`, `RPL_UMODEIS (221)`

---

### Message format rules (§2.3 / §2.3.1)

```
:<prefix> <command> <params> :<trailing>\r\n
```
- Messages are terminated by `\r\n` (CR-LF); max **512 characters** including CR-LF
- Prefix is optional for client→server messages (clients should not send one)
- Up to **15 parameters**; the last param may be prefixed with `:` to allow spaces
- **Partial data**: messages may arrive in chunks → buffer per client until `\r\n` is found (the nc test verifies this)
- Empty messages (bare `\r\n`) must be silently ignored

---

### Connection registration flow (§4.1 / §8.5)

Order the client must follow: `PASS` → `NICK` → `USER`  
Only after both NICK and USER are received is the client cDCF8D-DAFFA-925C3-2E410-56F0E-56302
onsidered registered.

On successful registration the server must send:
1. `001 RPL_WELCOME` — `:Welcome to the IRC network <nick>!<user>@<host>`
2. `002 RPL_YOURHOST` — server name and version
3. `003 RPL_CREATED` — server creation date
4. `004 RPL_MYINFO` — server name, version, available modes
5. MOTD sequence: `375 RPL_MOTDSTART`, `372 RPL_MOTD` lines, `376 RPL_ENDOFMOTD` (or `422 ERR_NOMOTD`)
6. LUSER stats: `251 RPL_LUSERCLIENT`, `252 RPL_LUSEROP`, `254 RPL_LUSERCHANNELS`, `255 RPL_LUSERME`

Unregistered clients must receive `451 ERR_NOTREGISTERED` for any command other than CAP/PASS/NICK/USER.

---

### JOIN success flow (§4.2.1)

After a successful JOIN, the server must send **to the joining client**:
1. `:nick!user@host JOIN #channel` — echo back the JOIN
2. `332 RPL_TOPIC` (if topic is set) or `331 RPL_NOTOPIC`
3. `353 RPL_NAMREPLY` — list of nicks in channel (prefixed `@` for chanops, `+` for voiced)
4. `366 RPL_ENDOFNAMES`

If the user creates the channel (first to join), they become the channel operator automatically.

Conditions that block JOIN:
- Channel is `+i` and user was not invited → `473 ERR_INVITEONLYCHAN`
- Channel has `+k` and wrong/missing key → `475 ERR_BADCHANNELKEY`
- Channel has `+l` and is full → `471 ERR_CHANNELISFULL`
- Client already on that channel → silently ignore (or re-send JOIN reply, depending on client)

---

### Channel operator privileges summary (§1.3.1)

Only chanops (`@`) may use: `KICK`, `INVITE` (on `+i` channels), `TOPIC` (on `+t` channels), `MODE`

A channel operator is identified by the `@` prefix on their nick in `NAMES`/`WHO`/`WHOIS` replies.

---

### Key numeric replies cheat-sheet

| Code | Name | When |
|------|------|------|
| 001 | RPL_WELCOME | Registration complete |
| 331 | RPL_NOTOPIC | No topic set on channel |
| 332 | RPL_TOPIC | Topic reply |
| 341 | RPL_INVITING | INVITE sent successfully |
| 353 | RPL_NAMREPLY | NAMES list entry |
| 366 | RPL_ENDOFNAMES | End of NAMES |
| 324 | RPL_CHANNELMODEIS | MODE query response |
| 401 | ERR_NOSUCHNICK | Nick not found |
| 403 | ERR_NOSUCHCHANNEL | Channel not found |
| 404 | ERR_CANNOTSENDTOCHAN | Can't send (mode +n/+m) |
| 405 | ERR_TOOMANYCHANNELS | Joined too many channels |
| 409 | ERR_NOORIGIN | PING/PONG missing origin |
| 421 | ERR_UNKNOWNCOMMAND | Unrecognised command |
| 431 | ERR_NONICKNAMEGIVEN | NICK with no param |
| 432 | ERR_ERRONEUSNICKNAME | Invalid nick chars |
| 433 | ERR_NICKNAMEINUSE | Nick already taken |
| 441 | ERR_USERNOTINCHANNEL | Target not in channel |
| 442 | ERR_NOTONCHANNEL | Sender not in channel |
| 443 | ERR_USERONCHANNEL | Already on channel (INVITE) |
| 451 | ERR_NOTREGISTERED | Command before registration |
| 461 | ERR_NEEDMOREPARAMS | Too few parameters |
| 462 | ERR_ALREADYREGISTRED | USER/PASS sent again |
| 464 | ERR_PASSWDMISMATCH | Wrong password |
| 467 | ERR_KEYSET | Key already set |
| 471 | ERR_CHANNELISFULL | Channel at +l limit |
| 472 | ERR_UNKNOWNMODE | Unknown mode character |
| 473 | ERR_INVITEONLYCHAN | Channel is +i |
| 475 | ERR_BADCHANNELKEY | Wrong channel key |
| 482 | ERR_CHANOPRIVSNEEDED | Not a chanop |