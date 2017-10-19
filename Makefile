all: shell.c 
	gcc -g -o shell shell.c

clean: 
	$(RM) shell