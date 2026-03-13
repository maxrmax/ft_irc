How to run subject example server
Usage:
make
./bircd <port number> <server password>  //./bird <1234> <secretpassword>
//in another terminal:
nc 127.0.0.1 <port number>
//now on server terminal it will tell, new client has joined


How to run our IRC server
./ircserv <port> <password>



How to run irssi client locally, on same machine
irssi
/connect 127.0.0.1 <port> <password>

How to run irssi client within LAN, any machine at school
ip a
//look for inet xxx.xxx.xxx.xxx
irssi
/connect <xxxx.xxxx.xxxx.xxxx> <port> <password>


How to run irssi client from another 42HN school computer
irssi
/connect <workstation like 2-E-4> <port> <password>


To see a direct message send from 1 client to 1 other client
/query <nickname of client to get message from>
at client, who sends message
/msg <nickname of client to send message to> <message text>
get out of query window
/unquery


/////////////////////////////
sending file within 42HN LAN
on server type into cli
ip -a
to get LAN ip look for something like 10.13.7.9 and use this in irssi to /connect 10.13.7.9 <port> <password>

at sender
/dcc send <target nick> "pathToFfile__StartingInsideHomedrive/filename"
i.e. 
/dcc send nickTo 'projects/rank5/ft_irc/text.txt"

at receiver you will get a message
DCC SEND from <sender nick> [<sender_client_ip> port <port>]: <filename> [<size>]
i.e.
DCC SEND from nickFrom [10.13.7.9 port 43239]: text.txt [2kB]

at receiver 
/dcc get <sender nick>
i.e. 
/dcc get nickFrom


on sending 
receiver will get message
DCC receiving file <filename> from <sender nick> [<sender_client_ip> port <port>]
i.e
DCC receiving file text.txt from nickFrom [10.13.7.9 port 33962]

sender will receive message
DCC sending file <filename> for <target nick> [<sender_client_ip> port <port>]
i.e
DCC sending file text.txt for nickTo [10.13.7.9 port 33962]


on success
receiver gets message 
DCC received file <filename> [<size>] from <sender nick> in <time needed to transfer> [<speed>]
i.e.
DCC received file text.txt [2kB] from nickFrom in 00:00:00 [1.01kB/s]

sender receives message
DCC sent file <filename> [<size>] for <target nick> in <time needed to transfer> [<speed>]
i.e.
DCC sent file text.txt [2kB] for nickTo in 00:00:00 [1.01kB/s]
/////////////////////////////