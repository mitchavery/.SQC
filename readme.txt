1. I have modified the p3_create.clp file on p3.total_balance view to include an alias 
on a.ID as accountID which affects the join query in ReportA 

select ID, Name, Gender, Age, Pin, TOTAL from p3.customer join p3.total_balance on accountID = p3.customer.id order by Total Desc;

2. To run this code, execute the following commands

db2 Connect to cs157a

db2 Prep p3.sqc

gcc -I../sqllib/include -c p3.c

gcc -o p3 p3.o -L../sqllib/lib  -ldb2 

./p3 db-1.properties 