#ifndef EXECUTION_H
#define EXECUTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fnmatch.h>
#include <dirent.h>

#include "parsing.h"
#include "main.h"

void run_commands(token* commands[], int* command_count);

#endif