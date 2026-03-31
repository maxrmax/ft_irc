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


<!-- ////////////////////////// -->
Readme Requirements
A README.md file must be provided at the root of your Git repository. Its purpose is
to allow anyone unfamiliar with the project (peers, staff, recruiters, etc.) to quickly
understand what the project is about, how to run it, and where to find more information
on the topic.
The README.md must include at least:
•The very first line must be italicized and read: This project has been created as part
of the 42 curriculum by <login1>[, <login2>[, <login3>[...]]].
•A “Description” section that clearly presents the project, including its goal and a
brief overview.
•An “Instructions” section containing any relevant information about compilation,
installation, and/or execution.
•A “Resources” section listing classic references related to the topic (documen-
tation, articles, tutorials, etc.), as well as a description of how AI was used —
specifying for which tasks and which parts of the project.
➠ Additional sections may be required depending on the project (e.g., usage
examples, feature list, technical choices, etc.).
Any required additions will be explicitly listed below.
Your README must be written in English.


<!-- Template to use -->
*This project has been created as part of the 42 curriculum by nsloniow, ngoyat, mring*

# Description
We created a irc server, to which you can connect per irssi (pre-installed on every 42 ubuntu pc),
or netcat (nc) which is just there on any *unix system.
It allows clients to communicate to each other, either through channels or per direct messages.
You are also able to send files per /dcc

# Instruction
make -> run

# Resources

---

## How to Run
either with `make run` -> default port is `6667` and server passwort is `start`
`make valgrind` for testing `6668`, `start`
./ircserver \<port> \<password> 

## Arguments
\<port> -> the port, starting from 1028
\<password> -> connection password for clients to use

## External functions used
yes

# additional section 1

# additional section 2

# additional section 3