/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:47:16 by nsloniow          #+#    #+#             */
/*   Updated: 2026/04/08 13:54:20 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "inputHandling.hpp"
#include <sys/resource.h>
#include <arpa/inet.h>
#include <algorithm>
#include <csignal>

extern volatile sig_atomic_t g_shutdown;

// client fd cleanup on discconect/close.
void clean_up(std::unordered_map<int, ClientUser> &clients)
{
    for (auto &[fd, client] : clients)
    {
        if (fd != -1)
        {
            if (close(fd) == 0)
                std::cout << "Client filedescriptor closed" << std::endl;
            else 
                std::cout << "Client filedescriptor could not be closed. Operating System will do this." << std::endl;
        }       
    }
    clients.clear();
}

// handles the dispatch of outgoing messages from the buffer to the recipient
static int sendMsg(ClientUser *clientUser)
{
    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "[08.01.01] sendMsg: " << clientUser->get_ClientUser_fd() << std::endl;
    #endif

    //keep sending until ouputBuffer is empty
    while (clientUser->get_outputBuffer().get_buffer().length() > 0)
    {
        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "[08.01.02] buffer before send: " << clientUser->get_outputBuffer().get_buffer() << std::endl;
        #endif
        
        // Send N bytes of BUF to socket FD. Returns the number sent or -1.
        // This function is a cancellation point and therefore not marked with __THROW.
        //limiting send size to 4096 if Buffer is too big, so not to overwheelm kernel, we send in chuncks
        // Typical OS socket buffer alignment:
        // Many OSes (Linux, BSD, macOS) internally use 4 KB pages for memory. Sending in multiples of 4096 bytes often aligns nicely with the kernel’s buffer pages.
        ssize_t send_len = send(clientUser->get_ClientUser_fd(), clientUser->get_outputBuffer().get_buffer().c_str(), std::min(clientUser->get_outputBuffer().get_buffer().size(), static_cast<size_t>(4096)), 0);
        
        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "[08.01.03] send_len: " << send_len << std::endl;
        #endif
        if (send_len < 0)
        {
            if (errno == EAGAIN)
            {
                // kernel cannot tae anything right now
                //so breaking to not overspin cpu
                break;
            }
            else
            {
                std::cerr << "Error sending to client.\n";
                return -1;
            }
        }
        else
        {
            #if defined(DEBUG_BUILD) && DEBUG_BUILD
                std::cout << "[08.01.04] outBUff BEFORE erase: " << __FILE__ << " " << __LINE__ << " " << clientUser->get_outputBuffer().get_buffer() << std::endl;
            #endif
            clientUser->get_outputBuffer().get_buffer().erase(0, send_len);
            #if defined(DEBUG_BUILD) && DEBUG_BUILD
                std::cout << "[08.01.05] outBUff AFRER erase: " << __FILE__ << " " << __LINE__ << " " << clientUser->get_outputBuffer().get_buffer() << std::endl;
            #endif
        }
        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "[08.01.06] buffer after send: " << clientUser->get_outputBuffer().get_buffer() << std::endl;
        #endif
    }
    return 0;
}

// process for the message to be sent to the receiving client
static int process_fd_ready_for_sending(Server &irc_server, int poll_index)
{
    int client_fd = irc_server.getPollFD()[poll_index].fd;
    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "[08.01] prepare sending. fd: " << client_fd << " - poll_index: " << poll_index << std::endl;
    #endif    
    if (client_fd != irc_server.get_server_fd())
    {
        //set client to client object that has been mapped to this fd 
        ClientUser *clientUser = irc_server.getClientByFd(client_fd);
        if (!clientUser)
            return -1;
        if (sendMsg(clientUser) < 0)
        {
            return -1;
        }
    }
    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "[08.02] message sent" << poll_index << std::endl;
    #endif  
    return 0;
}

int receive_message(Server &irc_server, int poll_index)
{
    int client_fd = irc_server.getPollFD()[poll_index].fd;
    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "[05.04.01] client_fd: " << client_fd << " - poll_index: " << poll_index << std::endl; // debug comment
    #endif
    ClientUser *clientUser = irc_server.getClientByFd(client_fd);
    if (!clientUser)
        return 0;

    char   msg[1024]; 
    int    read_len  = recv(client_fd, msg, sizeof(msg)-1, 0);

    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "[05.04.02] read_len: " << read_len << std::endl; // debug comment
        std::cout << "[05.04.02] Received: " << msg << std::endl;
    #endif

    if (read_len > 0)
    {
        msg[read_len] = '\0';
        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "[05.04.03] read_len: " << read_len << std::endl; // debug comment
            std::cout << "[05.04.04] Current Buffer: " << clientUser->get_inputBuffer().get_buffer() << std::endl;
        #endif

        clientUser->get_inputBuffer().append(msg);
    
        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "[05.04.04] Appended Buffer: " << clientUser->get_inputBuffer().get_buffer() << std::endl;
        #endif
    }
    else // read_len <= 0
    {   
        if (read_len == 0)
        {
            std::cout << "Client " << client_fd << " disconnected." << std::endl;
            // mark client for disconnect; runPoll will perform cleanup
            clientUser->setToDisconnect(true);
            return -1;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) // read_len < 0
        {
            #if defined(DEBUG_BUILD) && DEBUG_BUILD
                std::cout << "[05.04.05] errno: " << errno << std::endl;
            #endif
            // no data available right now, keep looping
            return 0;
        }

        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "[05.04.06] fatal error on fd: " << client_fd << std::endl;
        #endif
        // fatal socket error
        std::cerr << "recv error on fd " << client_fd << ": " << strerror(errno) << std::endl;
        // mark client for disconnect; runPoll will perform cleanup
        clientUser->setToDisconnect(true);
        return -1;  
    }
    return 0;
}

// Core logic around fd assignment and creation of client object in irc_server (_clients)
int acceptClientUser(Server &irc_server)
{
    sockaddr_in client_address_in = irc_server.get_server_address();
    socklen_t   client_address_length = sizeof(client_address_in);

    int client_accept_fd = accept(irc_server.get_server_fd(), (struct sockaddr *)&client_address_in, &client_address_length);

    if (client_accept_fd < 0)
    {
        // non-blocking needs check on errno so it keeps looping until we finish our loop
        if (errno == EAGAIN)
        {
            return -2; // sets client_waiting = false
        }
        else
        {
            std::cout << "Client acception failed." << std::endl;
            return -1;
        }
    }

    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "[05.03.01] runServer: Client accepted fd = " << client_accept_fd << std::endl;
    #endif

    // Make the client socket non-blocking
    fcntl(client_accept_fd, F_SETFL, O_NONBLOCK);
    
    // append client fd to poll list
    pollfd   temp;
    temp.fd         = client_accept_fd;
    temp.events     = POLLIN;
    temp.revents    = 0;
    irc_server.getPollFD().push_back(temp);
    irc_server.registerClientFd(client_accept_fd);

    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "[05.03.02] runServer: Client constructed: " << client_accept_fd << " with ip: " << irc_server.getClientIp(client_accept_fd) << std::endl;
    #endif

    irc_server.setClientIp(client_accept_fd, inet_ntoa(client_address_in.sin_addr));
   
    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "[05.03.03] runServer: Client " << client_accept_fd << " ip: " << irc_server.getClientIp(client_accept_fd) << std::endl;
    #endif
    
    return 0;
}

// accept all clients that are to be ready as per poll(fd) desicion
int clientUsers_waiting(Server &irc_server)
{
    bool clientUsers_are_waiting = true;

    while (clientUsers_are_waiting)
    {       
        // create sockets for our client/user, create Client object and create their fd.
        // accept_client return 0 -> accepted successfully
        switch (acceptClientUser(irc_server))
        {
            case -1:
                return -1; // failed to accept client
            case -2:
                clientUsers_are_waiting = false; 
                // no more clients are waiting right now, so we leave the unlimited loop to not block
        }
    }
    return 0;
}

// process the poll vector poll_fd
int process_ready_fd(Server &irc_server, int poll_index)
{
    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "[05.01] Poll_index " << poll_index << std::endl; // debug comment
    #endif
    int client_fd = irc_server.getPollFD()[poll_index].fd;
    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "[05.02] client_fd " << client_fd << std::endl; // debug comment
    #endif

    // if current polled_fd is the servers fd, check if any connection is waiting.
    if (client_fd == irc_server.get_server_fd())
    {
        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "[05.03] server_fd, check for waiting client: " << client_fd << std::endl; // debug comment
        #endif
        // checks if any connection is waiting
        clientUsers_waiting(irc_server);
    }
    else // else handle clients polled_fd and their input
    {
        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "[05.04] client_fd, receive message: " << client_fd << " - poll_index: " << poll_index << std::endl; // debug comment
        #endif
        if (receive_message(irc_server, poll_index) == -1)
            return -1;
        ClientUser *cu = irc_server.getClientByFd(client_fd);
        if (!cu)
            return -1;

        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "[05.05] handleClientInput: " << client_fd << std::endl; // debug comment
        #endif
        // it parses the input and then dispatches the input
        // command-dispatcher "handleClientCommands"
        handleClientInput(cu, irc_server);
    }
    return 0;
}

int runPoll(Server &irc_server)
{
    //setting POLLOUT in .events if outputBuffer is not empty
    for (size_t poll_index = 0; poll_index < irc_server.getPollFD().size(); poll_index++)
    {
        if (irc_server.getPollFD()[poll_index].fd != irc_server.get_server_fd())
        {    
            ClientUser *client_for_current_fd = irc_server.getClientByFd(irc_server.getPollFD()[poll_index].fd);            
            if (!client_for_current_fd->get_outputBuffer().get_buffer().empty())
            {
                irc_server.getPollFD()[poll_index].events |= POLLOUT;
            }
            else
            {
                #if defined(DEBUG_BUILD) && DEBUG_BUILD
                std::cout << "[09] no client/empty buffer events before &= ~: " << client_poll.events << std::endl;
                #endif
                irc_server.getPollFD()[poll_index].events &= ~POLLOUT;
                #if defined(DEBUG_BUILD) && DEBUG_BUILD
                    std::cout << "[09] no client/empty buffer events after &= ~: " << client_poll.events << std::endl;
                #endif
            }
        }
    }


    // poll()—Synchronous I/O Multiplexing
    // data() = pointer to first element

    // nfds (pollfd.size) is the count of entries
    // -1 is the timeout -> block until events occur

    // poll returns number based on:
    // 1+ (amount of fds with events)
    // 0 for timeout
    // -1 for error (example: nothing to poll -> EAGAIN)

    // With setting events on fd, we actually ask for permission. poll() tells us, when it got granted by kernel. We act with send() or recv() to do the deed.
    //poll()checks if the events, POLLIN, POLLOUT, POLLxx are happening on our fd. The kernel is showing the events are there. 
    //The kernel sets POLLIN when it received data from outside
    //Then we use recv() to actually take this data into our iputBufferContent.
    //The kernel will set POLLOUT when it is ready to send, can take data, has space to send.
    //Then we use send() to actually let the kernel take our outputBufferContent.
    int poll_amount = poll(irc_server.getPollFD().data(), irc_server.getPollFD().size(), -1);

    //kernel could not perform the poll operation at all
    if (poll_amount < 0)
    {
        // we return to the start of the loop and poll again
        // INTR = interuppted by signal
        // AGAIN = resource, here fd. temporarely unavailable
        if (errno == EAGAIN || errno == EINTR)
        {
            return 0;
        }
        // else if polling failed we break and exit the program
        else
        {
            std::cout << "Polling failed." << errno << std::endl;
            return -1;
        }
    }
    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "[01] Current Poll (amount of fd's with events) " << poll_amount << std::endl; // debug comment
    #endif

    // iterating all found fd's as long fd is within poll list
    // from here on we handle a single fd/client per iteration.
    // and where do i push fd's into the vector list?
    // -> process_ready_fd

    // double check fd (iterator) with every client_poll.fd
    //first read all data from all fds - as per rfc1459 8.3
    for (size_t poll_index = 0; poll_index < irc_server.getPollFD().size(); poll_index++)
    {
        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "[02] poll for fd " << poll_index << std::endl;
        #endif

        pollfd &client_poll = irc_server.getPollFD()[poll_index];

        ClientUser *client_for_current_fd = nullptr;
        if (client_poll.fd != irc_server.get_server_fd())
            client_for_current_fd = irc_server.getClientByFd(client_poll.fd);

        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "[03] client we poll " << client_for_current_fd << std::endl;
        #endif

        if (client_for_current_fd && client_for_current_fd->isToDisconnect())
        {
            int fd_to_remove = client_for_current_fd->get_ClientUser_fd();
            close(fd_to_remove);
            irc_server.unregisterClientFd(fd_to_remove);
            // swap-pop to remove current pollfd without invalidating iteration badly
            size_t last = irc_server.getPollFD().size() - 1;
            if (poll_index != last)
                irc_server.getPollFD()[poll_index] = irc_server.getPollFD()[last];
            irc_server.getPollFD().pop_back();
            // step back so the next iteration processes the moved entry
            if (poll_index != 0)
                --poll_index;
            continue;
        }


        if (client_poll.revents & (POLLERR | POLLHUP | POLLNVAL))
        {
            #if defined(DEBUG_BUILD) && DEBUG_BUILD
                std::cout << "[04] POLLERR | POLLHUP | POLLNVAL" << std::endl;
            #endif

            int fd = client_poll.fd;
            ClientUser *cu = irc_server.getClientByFd(fd);
            if (cu)
                cu->setToDisconnect(true);
            continue; // skip further processing
        }
    
        // bitwise AND check if POLLIN bit is inside .revents
        // if it is -> true
        // true means -> socket is ready for reading.
        // POLLIN -> socket read check
        if (client_poll.revents & POLLIN)
        {
            #if defined(DEBUG_BUILD) && DEBUG_BUILD
                std::cout << "[05] POLLIN fd " << poll_index << std::endl;
            #endif

            // reading data from client(s)
            int rc = process_ready_fd(irc_server, poll_index);

            //do not set revents to 0 as we need it for POLLOUT loop
            // client_poll.revents = 0;
            
            if (rc == -1)
            {
                poll_index--;
                continue;
            }
        }
    }

    //last write all data from all fds - as per rfc1459 8.3
    for (size_t poll_index = 0; poll_index < irc_server.getPollFD().size(); poll_index++)
    {
        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "[02] poll for fd " << poll_index << std::endl;
        #endif

        pollfd &client_poll = irc_server.getPollFD()[poll_index];

        ClientUser *client_for_current_fd = nullptr;
        if (client_poll.fd != irc_server.get_server_fd())
            client_for_current_fd = irc_server.getClientByFd(client_poll.fd);

        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "[03] client we poll " << client_for_current_fd << std::endl;
        #endif

        if (client_for_current_fd && client_for_current_fd->isToDisconnect())
        {
            int fd_to_remove = client_for_current_fd->get_ClientUser_fd();
            close(fd_to_remove);
            irc_server.unregisterClientFd(fd_to_remove);
            // swap-pop to remove current pollfd without invalidating iteration badly
            size_t last = irc_server.getPollFD().size() - 1;
            if (poll_index != last)
                irc_server.getPollFD()[poll_index] = irc_server.getPollFD()[last];
            irc_server.getPollFD().pop_back();
            // step back so the next iteration processes the moved entry
            if (poll_index != 0)
                --poll_index;
            continue;
        }


        if (client_poll.revents & (POLLERR | POLLHUP | POLLNVAL))
        {
            #if defined(DEBUG_BUILD) && DEBUG_BUILD
                std::cout << "[04] POLLERR | POLLHUP | POLLNVAL" << std::endl;
            #endif

            int fd = client_poll.fd;
            ClientUser *cu = irc_server.getClientByFd(fd);
            if (cu)
                cu->setToDisconnect(true);
            continue; // skip further processing
        }
    
        // POLLOUT -> socket write check  
        if (client_poll.revents & POLLOUT)
        {
            #if defined(DEBUG_BUILD) && DEBUG_BUILD
                std::cout << "[08] POLLOUT fd " << poll_index << std::endl;
            #endif
            // process the msg to send.
            if (process_fd_ready_for_sending(irc_server, poll_index) < 0)
            {
                // breaks runPoll loop if send returns -1
                return -1;
            }
            client_poll.revents = 0;
        }
    }
    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "[00] end of poll loop (for finished)" << std::endl;
    #endif
    return 0;
}

void init_server_limits(Server &irc_server)
{
    // reserver fd's for other programs
    const int reserved_fds = 10;
    // set a real limit
    struct rlimit rl;
    // soft limit
    size_t soft_limit = 0;
    // max sockets we can open by the system
    long sc = sysconf(_SC_OPEN_MAX);
    size_t max_clients = 0;

    // we get the soft limit
    if (getrlimit(RLIMIT_NOFILE, &rl) != 0 || rl.rlim_cur == RLIM_INFINITY)
        soft_limit = (sc > 0) ? (size_t)sc : 1024;
    else
        soft_limit = (size_t)rl.rlim_cur;

    // we define the max_clients to allow to connect
    if (soft_limit > (size_t)reserved_fds)
        max_clients = soft_limit - (size_t)reserved_fds;
    if (max_clients < 16)
        max_clients = 16;

    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "max_clients: " << max_clients << std::endl; // debug print
    #endif

    // Reserve space to avoid rehash/realloc during runtime.
    irc_server.getPollFD().reserve(max_clients + 1); // +1 for server/listener fd
    // also reserve the space on _clients map
    irc_server.get_clients_map().reserve(max_clients);
}

void runServer(Server &irc_server)
{
    // datastructure of a single fd with events (the polling fd)
    pollfd                  polling_server_fd;
    // first set server fd to poll list for server events
    polling_server_fd.fd         = irc_server.get_server_fd();
    
    // define the events we want the server to react to
    // For the server socket, you should NOT use POLLOUT.
    // A listening socket is almost always “writable”
    // POLLOUT causes poll() to wake up constantly => CPU burn
    // polling_server_fd.events     = POLLIN | POLLOUT;
    polling_server_fd.events     = POLLIN;
    
    // set to "no events have happened"
    polling_server_fd.revents    = 0;
    // push the server fd on the stack
    irc_server.getPollFD().push_back(polling_server_fd);

    // init the server
    init_server_limits(irc_server);

    while (!g_shutdown)
    {
        if (runPoll(irc_server) == -1)
            break;
    }
    std::cout << "\n" << "Graceful shutdown" << std::endl;
    clean_up(irc_server.get_clients_map());
}

