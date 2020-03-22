1. To run this code, execute the following commands

db2 Connect to cs157a

db2 Prep p3.sqc

gcc -I../sqllib/include -c p3.c

gcc -o p3 p3.o -L../sqllib/lib  -ldb2 

./p3 db-1.properties 
