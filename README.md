# Compilation steps
1.Install readline and pthread libraries (if u don't have them already) <br/>
2.Compile with this command: <br/> ```g++ shell.cpp -o shell -lreadline && g++ oscat.cpp -o oscat && g++ oshead.cpp -o oshead && g++ osenv.cpp -o osenv && g++ client.cpp -o client && g++ server.cpp -o server -lpthread``` <br/>
3.Run ```./shell```
## Features
##### Supports 4 custom commands: <br/>
  ```cat```    with options -b -E -n -s <br/>
  ```head```   with options -n -c -q -v <br/>
  ```env```    with option -u <br/>
  ```client``` (for the networking part) </br>
##### Supports piping between any number of commands
##### Supports redirection into files
##### Supports both redirection and piping in the same command
##### Supports multiple clients connected to a server
## Note
In order for the ```client``` command to work,you first need to run the server with the command ```./server``` .The default port is 5021.

## Some Pictures
![Imgur Image](http://i.imgur.com/klweaVl.jpg)
![Imgur Image](http://i.imgur.com/ydexq1b.jpg)
![Imgur Image](http://i.imgur.com/buS9qYX.jpg)
