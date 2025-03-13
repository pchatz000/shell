#ifndef PARSING_H
#define PARSING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

typedef enum token_type {
	UNDEFINED,
	COMMAND,		// ls, cd, pwd, cat ... 
	ARG,			// file names, flags ...
	IN_RD,			// the character '<'
	OUT_RD, 		// the character '>
	PIPE,			// the character '|'
	BG,				// the character '&'
	SC,				// semicolon
	AP_RD			// the string '>>'
	} token_type;

typedef struct token {
	char* s; 
	token_type type;
	struct token* next;
	struct token* input_rd;
	struct token* output_rd;
	struct token* append_rd;
	struct token* pipe;
	int bg;
} token;


void print_token(token* tk);

void add_command(token* tk);

void parse(char* line, token* commands[], int* command_count);

#endif