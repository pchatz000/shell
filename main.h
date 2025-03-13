#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#include "parsing.h"
#include "execution.h"

#define MAX_LINE_LENGTH 1000
#define HISTORY_MEMORY 5
#define MAX_ALIASES 100
// max commands includes background commands and multiple commands inline
#define MAX_COMMANDS 100 

#endif