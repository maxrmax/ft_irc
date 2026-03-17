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

# Instruction

## How to Run

## Arguments

## External functions used

# additional section 1

# additional section 2

# additional section 3