#include "main.h"


int main() {
	// the signals become ignored in the main process
	signal(SIGINT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	
	// the string to be printed every time
	char* prompt = "in-mysh-now$ ";
	
	// this array will hold all the lines by the user
	char lines[HISTORY_MEMORY][MAX_LINE_LENGTH];
	// the number of lines in the current session
	int lines_read = 0;
	
	// this stores the commands in one line
	// e.g. cat file ; ls -l is 2 commands
	// cat file | cut -f2 > test is 1 command
	token* commands[MAX_COMMANDS]; 
	while (1) {
		// periodically is called to ensure that the bg commands dont become zombies
		waitpid(-1, NULL, WNOHANG);
		
		int line_index = lines_read % HISTORY_MEMORY;

		char line[MAX_LINE_LENGTH];
		
		printf("%s", prompt);

		size_t n = MAX_LINE_LENGTH; 
		char* pointer = line;
		int line_length = getline(&pointer, &n, stdin);
		
		if (line_length == 1) continue;
		// removing the new line character
		line[--line_length] = '\0';

		if (strcmp(line, "exit") == 0) {
			break;
		}

		// here are parsed and executed some quick commands
		// these include myHistory and cd

		char quick_command[15];
		sscanf(line, "%s", quick_command);
		
		
		if (strcmp(quick_command, "myHistory") == 0) { // quick command my history
			if (strcmp(line, "myHistory") == 0) { // simple case, just print last commands
				for (int i=0; i<HISTORY_MEMORY; i++) {
					// in case we havent typed that many commands
					if (HISTORY_MEMORY > lines_read && i >= lines_read) break;
					int pos = (line_index-1-i + HISTORY_MEMORY) % HISTORY_MEMORY;

					printf("%d: %s\n", i, lines[pos]);
				}
				continue;
			}
			else {
				int num;
				sscanf(line, "%s %d", quick_command, &num);
				if (num<0 || num >= lines_read || num >= HISTORY_MEMORY) {
					printf("Couldnt execute command\n");
					continue;
				}
				int pos = (line_index-1-num + HISTORY_MEMORY) % HISTORY_MEMORY;
				strcpy(line, lines[pos]);
			}
		}


		if (strcmp(quick_command, "cd") == 0) {
			char* pointer = line + 3;
			if (!chdir(pointer))
				printf("%s\n", pointer);
			else 
				printf("error changing directory\n");
			continue;
		}
		
		// storing the command in history and seding its copy to be parsed
		// because the strtok function will ruin the original string

		strcpy(lines[line_index], line);
		lines_read++;

		int command_count = 0;

		// the line is parsed and split in tokens
		parse(line, commands, &command_count);

		// the commands get executed one by one
		run_commands(commands, &command_count);
		printf("\n");
		
	}

	return (0);
}

