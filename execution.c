#include "execution.h"


// this function checks if there is a wild character in a specific token

int wild_check(char* command_array[], int* command_size, token* tk) {
	int ret_value = 0;
	// looping through the directory to find a match 
	// i use fnmatch
	// all matches are stored in the array as strings
	DIR* directory;
	struct dirent* entry;
	int match_result;

	directory = opendir(".");
	if (directory == NULL) {
		fprintf(stderr, "Unable to open directory.\n");
		exit(1);
	}

	while ((entry = readdir(directory))) {
		match_result = fnmatch(tk->s, entry->d_name, 0);
		if (match_result == 0) {
			ret_value = 1;
			char* temp = malloc(strlen(entry->d_name)+1);
			strcpy(temp, entry->d_name);
			command_array[(*command_size)++] = temp; 
		}
	}

	closedir(directory);
	return ret_value;
}

// puts the command tree in a string array to be fed in execvp
// only command and arg type tokens are included

void command_expand(char* command_array[], int* command_size, token* tk) {
	command_array[(*command_size)++] = tk->s;
	for (tk = tk->next; tk != NULL; tk = tk->next) {
		if (tk->type == ARG && wild_check(command_array, command_size, tk) == 0) {
			command_array[(*command_size)++] = tk->s; 
		}
	}
	command_array[(*command_size)++] = NULL;
}

// takes a command token and does the necessary for its execution

void execute_command(token* command) {
	int pipeline = 0;
	// if the command is it be run in the background
	// in case of piped commands, all run in the bg
	int bg = command->bg;
	// we check if the command is a pipeline
	// if it is we have to do extra work

	// we also count the number of pipes
	for (token* tk = command; tk->pipe != NULL; tk=tk->pipe) { 
		pipeline++;
	}
	// this is to keep track of the commands in the fg
	int pids[MAX_COMMANDS];
	int pids_size = 0;

	int x = pipeline ? pipeline : 1;
	// we will need as many pipes as the pipeline
	int fd[x][2];

	for (int i=0; i<pipeline; i++) {
		pipe(fd[i]);
	}
	
	token* command_i = command;
	// N pipes means N+1 commands piped.
	// c1 | c2 | c3 | ... | cN
	

	// if there is no pipe, this loop runs only once
	int i=0;
	do {
		int pid = fork();
		if (pid == -1) 
			perror("couldnt execute command");
		else if (pid == 0) {
			// child process
			
			// reactivate the signals
			signal(SIGINT, SIG_DFL);
			signal(SIGTSTP, SIG_DFL);

			token* tk = command_i;
		
			char* command_array[50];
			int command_size = 0;

			// the command is put in a string array
			
			command_expand(command_array, &command_size, tk);

			// all the necessary redirections

			if (command_i->input_rd != NULL) {
				int input_fd = open(command_i->input_rd->s, O_RDONLY, 0777);
				dup2(input_fd, STDIN_FILENO);
			}
			if (command_i->output_rd != NULL) {
				int output_fd = open(command_i->output_rd->s, O_WRONLY | O_CREAT, 0777);
				dup2(output_fd, STDOUT_FILENO);
			}
			else if (command->append_rd != NULL) {
				int append_fd = open(command_i->append_rd->s, O_WRONLY | O_APPEND | O_CREAT, 0777);
				dup2(append_fd, STDOUT_FILENO);
			}
			if (pipeline) { // if the command is part of a pipeline
				// command_i->pipe == NULL means we are in the last process of the pipeline
				if (command_i->pipe != NULL) {	// we always redirect the output, except the last process
					// OUTPUT REDIRECTION
					dup2(fd[i][1], STDOUT_FILENO);
				}
				if (i != 0) {		// we always redirect the input, except the first process
					// INPUT REDIRECTION
					dup2(fd[(i-1)][0], STDIN_FILENO);
				}
				for (int j=0; j<pipeline; j++) {
					close(fd[j][0]);
					close(fd[j][1]);
				}
			}
			execvp(command_i->s, command_array);

			perror("couldnt execute command");
			exit(1);
		}
		else {
			pids[pids_size++] = pid; // keep all the processes ids
			command_i = command_i->pipe;
		}

		i++;
	} while (command_i != NULL);

	if (pipeline) {
		for (int i=0; i<pipeline; i++) {
			close(fd[i][0]);
			close(fd[i][1]);
		}
	}
	// wait for the processes to end, if they are not run in the background
	if (!bg) {
		for (int i=0; i<pids_size; i++) {
			waitpid(pids[i], NULL, 0);
		}
	}
}

// used to free resources, needs fix, TODO

void free_command_memory(token* tk) {
	if (tk == NULL) return;
	free_command_memory(tk->next);
	free_command_memory(tk->input_rd);
	free_command_memory(tk->output_rd);
	free_command_memory(tk->append_rd);
	free_command_memory(tk->pipe);
	free(tk);
}

// loops through the commands separated by a semicolon in a single line
// and executes them one by one

void run_commands(token* commands[], int* command_count) {
	for (int i = 0; i < *command_count; i++) {
		execute_command(commands[i]);
		//free_command_memory(commands[i]);
	}
}