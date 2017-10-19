#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

#define ARG_BUFSIZE 80
#define HIST_BUFSIZE 30
#define DELIM " \t\r\n\a"
#define CMDDELIM ";"

int commandCounter = 0;
int historyBuffer = 0;

/*
* Contains the arguments of every history entry,
* the number of arguments,
* if it has a '&'
*/
struct Entry{
	char ** args;
	int n;
	int background;
};

/*
* Takes history list as a parameter
* Checks if the history list needs to be realloced
* returns the resized list
*/
struct Entry ** checkSize(struct Entry ** history){
	if(commandCounter >= historyBuffer){
		int z = historyBuffer;
		historyBuffer += HIST_BUFSIZE;
		history = (struct Entry **)realloc(history,(historyBuffer * sizeof(struct Entry *)));	
		for( ; z < historyBuffer;z++){
			history[z] = (struct Entry *)malloc(sizeof(struct Entry));
			}
	}	
	return history;
}

/*
* Fills entry struct with args, num of args, and if it is has a '&'
*/
void addToHistory(char ** args,struct Entry ** history,int numOfArgs,int background){
	history[commandCounter]->args = args;
	history[commandCounter]->n = numOfArgs;
	history[commandCounter]->background = background;
	commandCounter++;
}

/*
*  Prints the history
*/
void getHistory(struct Entry ** history){
	int j,z;

	for(j = 0; j < commandCounter;j++){
		printf("%d ",j + 1);
		char ** oldargs = history[j]->args;
		int length = history[j]->n;
		for(z = 0; z < length; z++){
			printf("%s ",oldargs[z]);
		}
		if(history[j]->background == 1){
			printf("&");
		}
		printf("\n");
	}
}

/*
* Reads in the user input
*/
char * read_line(){
	char *line = NULL;
	ssize_t bufsize = 0;
	int num = getline(&line, &bufsize,stdin);
	return line;
}

/*
* Parses the command and returns the arguments as a char **
*/
char ** split_line(char * line,int* background,struct Entry ** history,int * flag,int *should_run){
	int buf_size = ARG_BUFSIZE;
	int i = 0;
	char ** args = (char **)malloc(buf_size * sizeof(char *));
	char * rest = line;
	char * arg = strtok_r(rest,DELIM,&rest);
	// loops through line using DELIM
	while(arg != NULL){
		//if the token contains "&", set background so parent does not wait
		if(strcmp(arg,"&") == 0){
			*background = 1;
			i--;
		}
		//if the first character is a !
		else if(arg[0] == '!'){
			*flag = 1;
			if(strlen(arg) != 1){
				if(arg[1] == '!'){
				//return previous command
					*flag = 0;
					if(commandCounter == 0){
						printf("No commands in history\n");
						return args;
					}
					addToHistory(history[commandCounter - 1]->args,history,history[commandCounter - 1]->n,history[commandCounter - 1]->background);
					if(strcmp(history[commandCounter - 1]->args[0],"history") == 0){
						*flag = 1;
					}
					if(history[commandCounter -1]->background == 1){
						*background = 1;
					}
					else{
						*background = 0;
					}
					return history[commandCounter - 1]->args;
				}
				else{
				//return nth command
					*flag = 0;
					arg++;
					int n = atoi(arg);
					if(n <= commandCounter){
						addToHistory(history[n - 1]->args,history,history[n - 1]->n,history[n - 1]->background);
						if(strcmp(history[n - 1]->args[0],"history") == 0){
							*flag = 1;
						}
						if(history[n - 1]->background == 1){
							*background = 1;
						}
						else{
							*background = 0;
						}
						return history[n - 1]->args;
					}
					printf("No such command in history\n");
					return args;
				}			
			}
		}
		//if the cmd is history, we do not want to evecvp on the args, so we set a flag
		else if(strcmp(arg,"history") == 0){
			*flag = 1;
			args[i] = arg;
		}
		//if we see exit, set should_run to 0 so it exits the loop
		else if(strcmp(arg,"exit") == 0){
			*should_run = 0;
		}
		else{
			args[i] = arg;
		}
		i++;
		//check if the args are too big, if so, realloc
		if(i >= buf_size){
			buf_size += ARG_BUFSIZE;
			args = (char **)realloc(args,buf_size * sizeof(char*));
		}
		arg = strtok_r(rest,DELIM,&rest);
	}
	addToHistory(args,history,i,*background);
	args[i] = 0;
	return args;
}

/*
* main function
*/
int main(void){
	int *should_run = malloc(sizeof(int));
	*should_run = 1;
	int pid;
	struct Entry ** history;
	char * line;
	char ** args;
	int i;

	history = (struct Entry **)malloc(HIST_BUFSIZE * sizeof(struct Entry *));
	historyBuffer = HIST_BUFSIZE;
	for(i = 0; i < HIST_BUFSIZE;i++){
		history[i] = malloc(sizeof(struct Entry));
	}
	//outer loops until exit is called
	while(*should_run){
		printf("osh>");
		fflush(stdout);
		line = read_line();
		char * cmds = strtok(line,CMDDELIM);
		//inner loop iterates through each emd separated by a ';'
		while(cmds != NULL){
			int *background = (int *)malloc(sizeof(int));
			int *flag = (int *)malloc(sizeof(int));
			*flag = 0;
			*background = 0;
			args = split_line(cmds,background,history,flag,should_run);
			history = checkSize(history);
			pid = fork();
			if(pid == 0){ 
				if(*flag == 0){
					int error = execvp(args[0],args);
					if(error == -1){
						printf("command not found!\n");
						exit(1);
					}
				}
				else if(*flag == 1){
					getHistory(history);
					exit(0);
				}
			}
			if(*background == 0){
				waitpid(pid,NULL,0);
			}
			else{
				//don't wait
			}
			free(background);
			free(flag);
			cmds = strtok(NULL,CMDDELIM);
		}
	}
	int j;
	for(j = 0;j < historyBuffer;j++){
		free(history[j]);
	}
	free(history);
	free(should_run);
	free(args);
	free(line);
	return 0;
}