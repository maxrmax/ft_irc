# runServer poll loop — analysis & notes

Short answer — is the code currently unsafe or just "what if" warnings?

- Immediate/likely problems (present now)
  - **Pointer invalidation from `poll_clientUser__mapping_via_fd`**
    - You store `ClientUser` by value in an `std::unordered_map` and take the address of the element (`&map[fd]`) to register elsewhere. `unordered_map` will rehash on growth and invalidate references/pointers to elements. This can produce use‑after‑free / UB once the map grows and rehashes.
  - **Skipped `pollfd` after `erase()`**
    - Erasing `getPollFD().erase(begin() + i)` during iteration shifts later elements left. The loop then increments `i` and may skip the element that moved into the erased slot.

- Not currently a bug (but watch out)
  - **`std::vector<pollfd>` reallocation risk**
    - Reallocation invalidates pointers/references to `pollfd` elements. Your current code indexes numerically each time (no long‑lived `pollfd*`/`&` held across `push_back`), so you are safe now. This is a "could become a problem" if code is changed to cache pointers or take refs across `push_back`.
  - **POLLERR / POLLHUP / POLLNVAL handling**
    - You only test `POLLIN`/`POLLOUT`. Errors/hangups should be checked first because they indicate the socket is unusable. This is robustness guidance rather than an immediate crash cause.

Concrete examples where problems appear

- unordered_map rehash
  1. Start with a small bucket count.
  2. Insert clients repeatedly; when insert growth crosses load_factor the map rehashes and moves elements.
  3. Any pointer taken earlier (from `&map[key]`) becomes dangling — later dereference causes UB/crash.

- erase + iterate skip
  - `pollfds = [listen, A, B, C]`. If `B` disconnects while iterating at index 2, code erases index 2. `C` moves to index 2, loop increments index to 3 and never processes `C` this iteration — lost events.

Minimal diagnostics (non-invasive)

- Log pointer values when `registerClientFd` stores a pointer; log again after subsequent inserts — unexpected changes indicate rehash invalidation.
- Reproduce by rapidly connecting/accepting many clients; crashes or corrupted behavior indicate map pointer invalidation.

Minimal mitigations (quick, non-invasive)

- Add a comment by the `registerClientFd(..., &map[key])` call warning about pointer invalidation risk.
- Call `getPollFD().reserve(expected_max_clients + 1)` at server init to reduce (but not eliminate) vector reallocations.
- Call `getPoll_clientUser__mapping_via_fd().reserve(expected_max_clients)` at server init to postpone unordered_map rehashes.

Better fixes (opt in when you want to refactor)

- Make map values heap‑allocated: `std::unordered_map<int, std::unique_ptr<ClientUser>>` and store raw pointers elsewhere (`ptr = map[fd].get()`). This guarantees stable addresses.
- Or maintain an `fd_to_index` map plus swap‑pop removal for the `pollfd` vector:
  - Append: push_back, fd_to_index[fd] = size-1
  - Remove: swap last into removed slot, update its index, pop_back, erase fd_to_index[removed_fd]

Checklist / next steps you can take (no forced changes)

- If you need a short, safe tweak now: reserve capacity for both containers in server init.
- If you plan to keep the current pointer usage long‑term: convert to owning pointers (unique_ptr) in the map.
- If you want lock‑free removals and to avoid skip bugs: add `fd_to_index` and swap‑pop erase implementation.

— End of file


---

Summary of every std::unordered_map I found and my recommendation

poll_clientUser__mapping_via_fd — defined in server.hpp (type currently std::unordered_map<int, ClientUser>)

How it's used:
runServer.cpp inserts ClientUser by value and immediately does registerClientFd(fd, &irc_server.getPoll_clientUser__mapping_via_fd()[fd]).
_clients, nick_clientUser, nicknames_history store ClientUser* that come from those addresses.
Risk: taking &map[key] while map stores values is unsafe because unordered_map can rehash and relocate elements → dangling pointers / UB.
Recommendation: convert this map to own heap objects:
std::unordered_map<int, std::unique_ptr<ClientUser>> poll_clientUser__mapping_via_fd;
Update call sites to create objects with std::make_unique<ClientUser>(...) and to pass stable pointers with .get() (e.g. registerClientFd(fd, poll_map[fd].get())).
Update getters to return the new map type/reference.
Priority: High — convert this one.
nicknames_history — std::unordered_map<std::string, ClientUser*> (in server.hpp)

How it's used:
Stores pointers to historical nick owners; uses ClientUser* but does not own them.
Recommendation:
Keep as pointer map (no need for unique_ptr here). After converting the poll map, ensure you set these pointers to poll_map[fd].get() or nullptr on disconnect.
Priority: Low — keep but ensure pointers point to stable owners.
nick_clientUser — std::unordered_map<std::string, ClientUser*>

How it's used:
Current nickname → ClientUser* lookup (used by NickIsAlreadyRegistered, getClientByNick).
Recommendation:
Keep as-is (pointer lookup). After poll-map conversion, set values to poll_map[fd].get() and clear them on disconnect.
Priority: Low.
_channels — std::unordered_map<std::string, Channel>

How it's used:
Channels stored by value; Server::getChannel/createChannel return Channel&.
Recommendation:
Leave as value unless you need stable Channel* elsewhere or want node-stability. If you do want stable pointers to channels, convert to unordered_map<string, unique_ptr<Channel>>. No immediate need in current code.
Priority: Low / Optional.
_clients — std::unordered_map<int, ClientUser*>

How it's used:
Active clients lookup for broadcasting and other server operations. Pointers point to client storage owned elsewhere.
Recommendation:
Keep as pointer map but ensure pointers refer to the stable owner (after poll-map conversion they should point to poll_map[fd].get()).
Priority: Low.
Other notes

getPoll_clientUser__mapping_via_fd() (server.cpp) returns std::unordered_map<int, ClientUser>& — this must be updated to return the unique_ptr map reference if you convert.
Minimal non-invasive mitigation if you don't convert now:
poll_clientUser__mapping_via_fd().reserve(expected_clients) at server init to postpone rehashes (not a guarantee).
Add clear comments and audits where &map[...] is taken.
If you convert poll_clientUser__mapping_via_fd:
Update insertions:
From map[fd] = client_created; to map[fd] = std::make_unique<ClientUser>(client_accept_fd); (or emplace).
Set fields (setIp, etc.) on the allocated object before/after insertion.
Update access sites:
ClientUser &u = map[fd]; → ClientUser &u = *map.at(fd); or ClientUser* p = map.at(fd).get();
registerClientFd(fd, &map[fd]) → registerClientFd(fd, map.at(fd).get());
Update getters in server.hpp and server.cpp signatures and any callsites.
Update cleanup: erase entries; unique_ptr destructor will free object.