#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE 80
#define ARG_BUFSIZE = 80
#define DELIM " "



char * read_line(void){
	char *line = NULL;
	ssize_t bufsize = 0;
	getline(&line, &bufsize,stdin);
	return line;
}

char ** split_line(char * line){
	int buf_size = ARG_BUFSIZE;
	int i = 0;
	char ** args = malloc(buf_size * sizeof(char *));
	char * arg = strtok(line,DELIM);

	while(arg != NULL){
		args[i] = arg;
		i++;

		if(i >= buf_size){
			buf_size += ARG_BUFSIZE;
			args = realloc
		}
	}


}

int main(void){
	char * args[MAX_LINE/2 + 1];
	int should_run = 1;

	while(should_run){
		printf("osh>");
		fflush(stdout);

		char * input; 
		gets(input);

		int i =0;
		args[i] = strtok(input," ");

		while(args[i]!=NULL){
			array[++i] = strtok(NULL," ");
		}



		if(fork() == 0){

		}

	}

	return 0;
}