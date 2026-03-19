Things we have TODO

1.
InputBuffer
-> If the client sends more than 512 bytes total without a CRLF, the connection must be closed.
not properly handled yet.

2. OutputBuffer comment
//TODO integration with recv()/poll() loop
-> is that still true or is it integrated in runServer.cpp?

3. Header file includes
check all header files for circular includes, redundant inclusions and clean up the order(s)

4. ClientUser (*ToDisconnect)
a client writing quit (in netcat) is not closing the fd.
the server must close the fd in eventhandler (poll())
i first thought i manually close the fd but thats apparently not good practice bc of cleanup.
-> check cleanup pipeline on exit
-> check when and where server closes fd.

5. if connecting to server per localhost/127.0.0.1
5.1. file transfer wont work, get will pull localhost/127.0.0.1
5.2 the wrong ip address is saved, it should grab the proper network address
--> idk where or how we get it but its in ifconfig the first non virtual interface:
--> eno1 in 42 school system, usually its eth0 or wlan0

6. CmdNick refactor
-> i think isReadyToRegister is redundant and could be refactored away

7. OutputBuffer::append and refactors (general)
-> buffer append always puts \r\n at the end. (clear + newline)
-> means we can remove it EVERYWHERE else.
-> removing dead code in class

8. quit flag?
-> might need a quit flag for commandDispatcher until poll() closes connection

9. 