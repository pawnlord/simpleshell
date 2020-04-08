# simpleshell
A very very simple C shell for Linux.  

## building
You can use the build script through `. build.sh` or `source build.sh`,  
If you are a make type of guy or not on bash, use the makefile like  
```
mkdir bin
make
```

## usage
```shell
./simpleshell.o [<command...>]
```
Currently, you just run the shell executible (usually simpleshell.o, but you can change it in the build script)  
You can also run a single command through it, by just putting the command in the arguements without any quotes.  

## commands
Not many right now, but there are 2:
- **cd:** change directory. **args:** directory to go change to.  
- **exit:** close the shell
- **listprocesses:** list all background processes  

## syntax
Like all good shells, there's a bit of syntax  
currently, there's not much but here you go:  
- **&:** put this at the end and it becomes a background process  
- **~:** home directory when at start of command or arguement  

## TODO
- fix bugs  
	- seg faults
