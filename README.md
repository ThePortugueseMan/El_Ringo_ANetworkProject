# Project Description



<p align="center">
  <img max-width="300" src="https://github.com/ThePortugueseMan/El_Ringo_ANetworkProject/blob/main/Docs/Ring.png">
</p>


This project simulates the creation, and interaction of a ring of nodes.

Each node (red square) has a key value, and it's responsible for all the key values between itself and the next node (grey circles). For example, if there are 2 nodes one with key 10 and the next one with key 20, then the objects with key 10-19 are responsability of node with key 10.

Each nodes comunicates to it's neighbour via TCP (line).

A node can be connected to any other node through a shortcut, using an UDP connection (dotted line). 



# Supported Commands

make -  compiles the program and makes an executable

./main - runs the program and creates the inital node on port number printed

./main [ip] - runs the program on this specific ip

./main [port] - creates  a node on [port] (  each node is a server and also a client so it must run in independent terminals, you can use ubuntu for windows 10 and just open multiple terminals )

new - connects the node to itself ( next udp and tcp port will be set to itself )

pentry [key] [ip] [port] - node [key] enters the ring between node in [port] and its successor, with previous ring being set to the one in port [port] and next node its successor

find [key] - finds the node responsible for [key]

bentry [ip] [port] - asks the ring the information of the ring that he should be doing pentry in ( finds the node with key more close to it´s own key ) 

leave - the node leaves the ring

chord [key] [ip] [port] - creates a UDP shortcut to node [key] 

exit - closes the program
