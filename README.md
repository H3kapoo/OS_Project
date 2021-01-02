# Compilation steps
1.Install readline and pthread libraries (if u don't have them already) <br/>
2.Compile with this command: <br/> ```g++ shell.cpp -o shell -lreadline &&``` <br/> ```g++ oscat.cpp -o oscat && ```<br/>```g++ oshead.cpp -o oshead &&```<br/>``` g++ osenv.cpp -o osenv && ```<br/>```g++ client.cpp -o client && ```<br/>```g++ server.cpp -o server -lpthread``` <br/>
3.Run ```./shell```
## Note
In order for the ```client``` command to work,you first need to run the server with the command ```./server``` .The default port is 5021.
