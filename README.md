# producer-worker-socket
A multithreaded socket server and client with work queue
https://github.com/fdpeiter/producer-worker-socket

This project consists of two programs:

client
server

The "client" it's a multithreaded program that creates connections to the "server", each of this connections can be a reader or a writer that will access an internal variable of the server;
This internal variable keeps track of the number of the writes performed. Each reader gets this value from the server and each writer increments this value from the server, then get the new value from the server.

Usage:

server <ip_address> <port>
client <ip_address> <port> <num_connections> <num_threads>

The num_connections must be a pair value, so it can produce the same amount of readers and writers.


WIP
