#include "parsing.h"

// used for debugging

void print_token(token* tk) {
	char* enum_to_string[] = {"UNDEFINED", "COMMAND", "ARG", "IN_RD", "OUT_RD", "PIPE", "BG", "SC", "AP_RD"};

	printf("%-10s,  \"%s\" \n",  enum_to_string[tk->type], tk->s);
	printf("|--> \t next:\"%s\", bg:%d\n", tk->next==NULL ? "nan" : tk->next->s, tk->bg);
	if (tk->pipe != NULL) printf("|--> \t pipe to: \"%s\" \n", tk->pipe->s);
	if (tk->input_rd != NULL) printf("|--> \t input from: \"%s\" \n", tk->input_rd->s);
	if (tk->output_rd != NULL) printf("|--> \t output from to: \"%s\" \n", tk->output_rd->s);
	printf("\n");
}

// used for debugging

void add_command(token* tk) {
	printf("added_command: %s\n\n\n", tk->s);
}

// parses the string and creates the token struct

void parse(char* line, token* commands[], int* command_count) {
	char* str;

	str = strtok(line, " ");
	int count = 0; // the number of tokens
	token_type last = UNDEFINED; // what we need to have as the next token
	token* tokens[150];
	// token split
	while ( str != NULL ) {
		token* tk = malloc(sizeof(token));
		tk->s = str;
		if (strcmp(str, "<") == 0) {
			tk->type = IN_RD;
		}
		else if (strcmp(str, ">") == 0) {
			tk->type = OUT_RD;
		}
		else if (strcmp(str, "|") == 0) {
			tk->type = PIPE;
		}
		else if (strcmp(str, "&") == 0) {
			tk->type = BG;
		}
		else if (strcmp(str, ";") == 0) {
			tk->type = SC;
		}
		else if (strcmp(str, ">>") == 0) {
			tk->type = AP_RD;
		}
		else {
			tk->type = ARG;
		}

		if (last == UNDEFINED || last == PIPE || last == BG || last == SC) {
			if (tk->type == ARG) tk->type = COMMAND;
		}

		tokens[count++] = tk;
		last = tk->type;
		str = strtok(NULL, " ");
	}
	// creation of the command tree
	token* cur_command = NULL;
	token* last_arg = NULL;
	token* first_command = NULL;
	for (int i=0; i<count; i++) {
		if (tokens[i]->type == COMMAND) { 
			last_arg = tokens[i];
			if (i == 0 || tokens[i-1]->type == SC || tokens[i-1]->type == BG) {
				commands[(*command_count)++] = tokens[i];
				first_command = tokens[i];
				//add_command(tokens[i]);
			}
			else if (tokens[i-1]->type == PIPE) {
				cur_command->pipe = tokens[i];
			}
			cur_command = tokens[i];
		}
		
		if (tokens[i]->type == ARG) {
			if (tokens[i-1]->type == IN_RD) {
				cur_command->input_rd = tokens[i];
			}
			else if (tokens[i-1]->type == OUT_RD) {
				cur_command->output_rd = tokens[i];
			}
			else if (tokens[i-1]->type == AP_RD) {
				cur_command->append_rd = tokens[i];
			}
			else {
				last_arg->next = tokens[i];
				last_arg = tokens[i];
			}
		}

		if (tokens[i]->type == BG) {
			first_command->bg = 1;
		}
	}


	// printf("next step\n");
	// for (int i=0; i<count; i++) {
	// 	print_token(tokens[i]);
	// }
	// for (int i=0; i<count; i++) {
	// 	if (tokens[i]->type != COMMAND && tokens[i]->type != ARG)
	// 		free(tokens[i]);
	// }
}