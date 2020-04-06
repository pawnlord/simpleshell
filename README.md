# simpleshell
A very very simple C shell for Linux.  

## building
Just use the build script through `. build.sh` or `source build.sh`,  

## usage
```shell
./simpleshell.o [<command...>]
```
Currently, you just run the shell executible (usually simpleshell.o, but you can change it in the build script)  
You can also run a single command through it, by just putting the command in the arguements without any quotes.  

## Commands
Not many right now, but there are 2:
- **cd:** change directory. **args:** directory to go change to.  
- **exit:** close the shell

## TODO
- background tasks  
- fix bugs  
	- Arguements not fully clearing after every command  
	- Seg faults