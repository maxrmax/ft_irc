How to run subject example server
Usage:
make
./bircd <port number> <server password>  //./bird <1234> <secretpassword>
//in another terminal:
nc 127.0.0.1 <port number>
//now on server terminal it will tell, new client has joined


How to run our IRC server
./ircserv <port> <connection password>



How to run irssi client locally, on same machine
irssi
/connect 127.0.0.1<port>

How to run irssi client within LAN, any machine at school
ip a
//look for inet xxx.xxx.xxx.xxx
irssi
/connect <xxxx.xxxx.xxxx.xxxx> <port>