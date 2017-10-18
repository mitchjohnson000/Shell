#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>


#define MAX_LINE 80
#define ARG_BUFSIZE 80
#define HIST_BUFSIZE 30
#define DELIM " \t\r\n\a"
#define CMDDELIM ";"

int commandCounter = 0;
int historyBuffer = 0;

struct Entry{
	char ** args;
	int n;
};

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
void addToHistory(char ** args,struct Entry ** history,int numOfArgs){
	history[commandCounter]->args = args;
	history[commandCounter]->n = numOfArgs;
	commandCounter++;
}

char * read_line(){
	char *line = NULL;
	ssize_t bufsize = 0;
	int num = getline(&line, &bufsize,stdin);
	return line;
}

char ** split_line(char * line,int* background,struct Entry ** history,int * flag,int *should_run){
	int buf_size = ARG_BUFSIZE;
	int i = 0;
	char ** args = (char **)malloc(buf_size * sizeof(char *));
	char * rest = line;
	char * arg = strtok_r(rest,DELIM,&rest);

	while(arg != NULL){
		if(strcmp(arg,"&") == 0){
			*background = 1;
		}else if(arg[0] == '!'){
			*flag = 1;
			if(strlen(arg) != 1){
				if(arg[1] == '!'){
				//return previous command
					*flag = 0;
					if(commandCounter == 0){
						return args;
					}
					addToHistory(history[commandCounter - 1]->args,history,history[commandCounter - 1]->n);
					return history[commandCounter - 1]->args;
				}
				else{
				//return nth command
					*flag = 0;
					arg++;
					int n = atoi(arg);
					if(n < commandCounter){
						addToHistory(history[n]->args,history,history[n]->n);
						return history[n]->args;
					}
					return args;
				}			
			}
		}
		else if(strcmp(arg,"history") == 0){
			*flag = 1;
			int j,z;
			for(j = 0; j < commandCounter;j++){
				printf("%d ",j);
				char ** oldargs = history[j]->args;
				int length = history[j]->n;
				for(z = 0; z < length; z++){
					printf("%s",oldargs[z]);
				}
				printf("\n");
			}
			args[i] = arg;
		}
		else if(strcmp(arg,"exit") == 0){
			*should_run = 0;
		}
		else{
			args[i] = arg;
		}

		i++;
		if(i >= buf_size){
			buf_size += ARG_BUFSIZE;
			args = (char **)realloc(args,buf_size * sizeof(char*));
		}
		arg = strtok_r(rest,DELIM,&rest);
	}
	addToHistory(args,history,i);
	args[i] = 0;

	return args;
}

int main(void){
	int *should_run = malloc(sizeof(int));
	*should_run = 1;
	int pid;
	struct Entry ** history;
	char * line;
	char ** args;

	history = (struct Entry **)malloc(HIST_BUFSIZE * sizeof(struct Entry *));
	historyBuffer = HIST_BUFSIZE;
	int i;

	for(i = 0; i < HIST_BUFSIZE;i++){
		history[i] = malloc(sizeof(struct Entry));
	}


	while(*should_run){
		printf("osh>");
		fflush(stdout);
		line = read_line();

		char * cmds = strtok(line,CMDDELIM);
		while(cmds != NULL){
			int *background = (int *)malloc(sizeof(int));
			int *flag = (int *)malloc(sizeof(int));
			*flag = 0;
			*background = 0;

			args = split_line(cmds,background,history,flag,should_run);

			history = checkSize(history);


			if(*flag == 0){
				pid = fork();
				if(pid == 0){
					if(execvp(args[0],args) == -1){
						printf("command not found!");
						kill(getpid(),SIGTERM);
					}
					free(line);
					free(args);
				}
				if(*background == 0){
					waitpid(pid,NULL,0);
				}else{
					printf("PID %d\n", pid);
				}
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

	return 0;
}