This is a project simulating the creation, and interaction of a ring of nodes. This nodes comunicate to it´s neighours via TCP and also has shortcuts to other node´s using UDP. Each node has a key , and it also takes care of the keys that are near him so for example, if there are 2 nodes one with key 10 and the next one with key 20, then the objects with key 10-19 are responsability of node with key 10.
The comands this program supports are:
make -  compiles the program and makes an executable
./main - runs the program and creates the inital node on port number printed
./main 127.0.1.1 - runs the program on this specific ip ( if you want to run it on a different one from your pc )
./main 58001 - creates  a node on port 58001 (  each node is a server and also a client so it must run in independent terminals, you can use ubuntu for windows 10 and just open multiple terminals )
new - connects the node to itself ( next udp and tcp port will be set to itself )
pentry 10 127.0.1.1 58001 - current node enters the ring between node in port 58001 and its successor, with previous ring being set to the one in port 58001 and next node its successor
find 10 - finds wich node is responsible for key 10
bentry 127.0.1.1 58001 - asks the ring the information of the ring that he should be doing pentry in ( finds the node with key more close to it´s own key ) 
leave - the node leaves the ring
chord 10 127.0.1.1 58001 - creates a UDP shortcut to node 10 
exit - closes the program
