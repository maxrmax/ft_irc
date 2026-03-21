Messy TODO:

Some parts are already addresses, TODO has not been updated after that (yet).


a client writing quit (in netcat) is not closing the fd.
the server must close the fd in eventhandler (poll())
i first thought i manually close the fd but thats apparently not good practice bc of cleanup.

```cpp
/*
TODO: runServer.cpp disconnect/poll refactor checklist

1) Centralize disconnect flow
   - Create one helper for: unregisterClientFd(fd), erase client map entry, close(fd), erase pollfd index.
   - Reuse this helper for both QUIT-triggered disconnect and recv()==0 (EOF).

2) Return status when current poll index is erased
   - receive_message/process_ready_fd should return:
     - <0 on error
     - >0 when current fd was removed from poll list
     - 0 otherwise
   - Prevent continuing to use poll_fd[fd] after erase.

3) Avoid invalid access after erase
   - Current flow may call handleClientInput(...) after receive_message removed poll_fd[fd].
   - Guard against stale index/out-of-bounds and stale map lookups.

4) Handle QUIT transport teardown in network layer
   - CmdQuit may unregister server state, but socket still needs close + poll erase here.
   - After handleClientInput(...), detect "client no longer registered" and disconnect current fd.

5) Fix runPoll iteration when erasing elements
   - Do not use unconditional ++fd after erase of current index.
   - Use manual increment strategy with an "erasedCurrent" flag.

6) Avoid accidental map insertion via operator[]
   - Replace poll_clientUser__mapping_via_fd[fd] reads with find()/at() where possible.
   - Prevent recreating client entries after disconnect paths.

7) Keep EOF and QUIT behavior identical
   - Same teardown path for recv()==0 and QUIT.
   - Ensures consistent cleanup of channels, nick maps, socket, and poll vector.

8) Optional safety checks
   - Handle POLLERR/POLLHUP/POLLNVAL with same disconnect helper.
   - Ensure close(fd) is not called twice in shutdown/cleanup paths.
*/
```

--- 
Comprehensive project TODO

Purpose
- This file collects actionable issues, bugs, refactors, and improvements found across the repository so you can work through them offline.

How to use
- Work top-to-bottom; high-priority correctness and resource-leak items are first. Each item lists the area, symptoms, suggested change, and a priority.

HIGH PRIORITY — correctness, crashes, resource leaks
- InputBuffer (512-byte rule):
	- Area: `src/network/client_user/InputBuffer.cpp`
	- Symptom: If a client sends >512 bytes without CRLF the connection must be closed per IRC rules; current implementation doesn't enforce this consistently.
	- Action: Enforce a 512-byte limit on accumulated bytes without CRLF. When exceeded, mark `ClientUser` to disconnect and ensure fd is closed by the central poll loop. Add tests.
	- Priority: High

- Client disconnect lifecycle and fd closing:
	- Area: `src/network/server/runServer.cpp`, `includes/ClientUser.hpp`, `src/network/client_user/ClientUser.cpp`
	- Symptom: Clients that issue QUIT or otherwise disconnect sometimes leave open fds; ownership of socket close is ambiguous.
	- Action: Centralize socket close and full cleanup in the poll event handling code. Remove manual closes from command handlers. Ensure channel membership and global registries are cleaned before fd is freed.
	- Priority: High

- OutputBuffer / non-blocking send integration:
	- Area: `src/network/client_user/OuputBuffer.cpp`, `src/network/server/runServer.cpp`
	- Symptom: Output buffer integration with poll write events is incomplete; CRLF handling is duplicated across code.
	- Action: Implement write-ready handling: attempt to send buffer on POLLOUT, keep unsent tail, and only remove client when buffer fully flushed (if necessary). Centralize CRLF behavior (append only in one place or provide explicit appendLine). Consider renaming `OuputBuffer` -> `OutputBuffer` (spelling).
	- Priority: High

- NICK handling and nickname registry:
	- Area: `src/commands/CmdNick.cpp`, `summary.md`
	- Symptom: Nick-change broadcast is commented out; nick registry may retain old nicks on change.
	- Action: Implement nickname validation, collision checks, server numeric replies, and broadcast nick changes to other registered clients. Ensure old nick removed from registry.
	- Priority: High

- Local IP detection vs loopback:
	- Area: `src/network/server/runServer.cpp`
	- Symptom: Server may return loopback address for operations that require externally reachable IP (e.g., file transfer), causing wrong addresses to be stored/sent.
	- Action: Detect non-loopback interface IP or allow explicit configuration of advertised IP. Add fallback behavior and clear logging when using loopback.
	- Priority: High

MEDIUM PRIORITY — RFCs, features, and robustness
- MODE command implementation:
	- Area: `src/commands/CmdMode.cpp`
	- Symptom: Not implemented according to RFC 1459.
	- Action: Implement necessary MODE semantics for server scope (channel modes commonly used), or document unsupported parts.
	- Priority: Medium

- QUIT command behavior and notifications:
	- Area: `src/commands/CmdQuit.cpp`
	- Symptom: QUIT handling incomplete (TODO noted); may not broadcast to channels or clean up state correctly.
	- Action: Broadcast QUIT message to appropriate clients, remove client from channels, and mark for server-side fd close.
	- Priority: Medium

- TOPIC command behavior:
	- Area: `src/commands/CmdTopic.cpp`
	- Symptom: Incomplete TODO; topic set/get semantics and permission checks unclear.
	- Action: Implement topic set/get and operator-only restrictions if required by project scope.
	- Priority: Medium

LOW / MAINTENANCE PRIORITY — hygiene & refactors
- Header include hygiene and circular includes:
	- Area: `includes/*` and `src/*`
	- Symptom: Potential circular and redundant includes.
	- Action: Audit headers, use forward declarations where possible, and normalize include order. Fix circular includes by moving dependencies to cpp files.
	- Priority: Medium

- Rename `OuputBuffer` -> `OutputBuffer` (spelling):
	- Area: `src/network/client_user/OuputBuffer.cpp`, `includes/OutputBuffer.hpp` (if present)
	- Symptom: Typo in class/file name hampers readability.
	- Action: Optional refactor/rename in a single, well-reviewed commit.
	- Priority: Low

- Simplify registration flow (CmdNick / isReadyToRegister):
	- Area: `src/commands/CmdNick.cpp`
	- Symptom: `isReadyToRegister` may be redundant; registration logic spread across modules.
	- Action: Consolidate state checks into one registration handler and remove redundant flags.
	- Priority: Low

- OutputBuffer::append CRLF duplication:
	- Area: Output buffer and all command handlers that append lines
	- Symptom: Both buffers and call-sites append CRLF leading to inconsistency.
	- Action: Decide ownership of CRLF (buffer should standardize appendLine vs appendRaw) and update callers.
	- Priority: Low

- Remove commented/dead code and stale TODO comments:
	- Area: `src/commands/CmdPass.cpp` and scattered TODOs
	- Action: Remove or convert to tracked issues; keep implementation minimal and clean.
	- Priority: Low

TESTING, DOCUMENTATION, DEPLOYMENT
- Add tests for core flows:
	- Areas: registration (PASS/USER/NICK), nick-change broadcasts, JOIN/PART, PRIVMSG routing, QUIT/disconnect behavior, 512-byte InputBuffer rule, output buffer partial sends.
	- Action: Create small test scripts (Python or shell) and CI jobs to run them. If desired, create a lightweight test harness in `tests/`.
	- Priority: Medium

- Update docs to match implemented subset of IRC:
	- Area: `Documentation.md`, `summary.md`, `readme.md`
	- Action: Document supported commands, known limitations (e.g., partial MODE subset), and how the server determines advertised IP.
	- Priority: Low

SECURITY & ROBUSTNESS
- Input validation and sanitization:
	- Area: Parser and all `src/commands/*`
	- Symptom: Missing or inconsistent parameter validation may allow malformed commands to cause undefined behavior.
	- Action: Add strict checks (length limits, valid characters), return numeric error codes on failure.
	- Priority: High

- Proper non-blocking I/O handling:
	- Area: All network code in `src/network/*`
	- Symptom: Partial sends/receives need robust EAGAIN/EWOULDBLOCK handling and correct buffer retention.
	- Action: Audit all recv/send usage paths and ensure correct behavior on non-blocking sockets.
	- Priority: High

PERFORMANCE
- Avoid heavy linear scans on common code paths:
	- Area: broadcasting and nickname lookups
	- Action: If needed, use maps/sets for nickname lookup and maintain subscriber lists for channels to reduce full-list scans.
	- Priority: Low

CODE STYLE / CLEANLINESS
- Standardize naming and formatting:
	- Areas: repository files; class/function naming
	- Action: Small style pass; consider running `clang-format` or equivalent and keeping changes minimal.
	- Priority: Low

QUICK REFERENCE OF LOCATED TODO COMMENTS
- `includes/ClientUser.hpp` — disconnect flags and behavior
- `src/network/client_user/InputBuffer.cpp` — input limit and poll integration
- `src/network/client_user/OuputBuffer.cpp` — output buffer TODOs
- `src/network/server/runServer.cpp` — IP detection and disconnect/poll checklist
- `src/commands/CmdNick.cpp` — nickname validation/broadcast TODOs
- `src/commands/CmdMode.cpp` — missing MODE implementation
- `src/commands/CmdTopic.cpp`, `src/commands/CmdQuit.cpp` — TODO markers
- `src/network/channel/Channel.cpp` — auto-op selection TODO

PRIORITIZED NEXT STEPS (small actionable batch):
1. Enforce InputBuffer 512-byte rule and add unit test.
2. Centralize disconnect handling and ensure poll loop closes fds and runs cleanup.
3. Integrate OutputBuffer with POLLOUT and unify CRLF handling.
4. Implement NICK validation and broadcast.

If you'd like, I can start implementing step 1 now and open a PR with tests and a short changelog.