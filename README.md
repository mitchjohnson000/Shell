CS 352 Project 1
================
Created by Mitchell Johnson

Features
--------

* Enter make to compile the shell.
* The number of arguments is unlimited (or until your memory is full), as it uses dynamic allocation.
* The history is also unlimted as it is dynamically allocated. 
* Separating commands is done by using a ';' example: "ls -la;ping google.com -c 5 &"
* Piping is not implemented on this version of the shell.
* '!! returns most recent command, and '!N' returns the nth command. 
* Adding a '&' argument to a command tells the parent process not to wait for the child process.