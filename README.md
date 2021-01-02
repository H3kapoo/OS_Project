# Compilation steps
1.Install readline and pthread libraries (if u don't have them already) <br/>
2.Run this in the terminal: <br/> ```g++ shell.cpp -o shell -lreadline && g++ oscat.cpp -o oscat && g++ oshead.cpp -o oshead && g++ osenv.cpp -o osenv && g++ client.cpp -o client && g++ server.cpp -o server -lpthread``` <br/>

## Note
In order for the ```client``` command to work,you first need to run the server with the command ```./server``` .The default port is 5021.
