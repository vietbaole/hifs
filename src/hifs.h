#pragma once

#include <time.h>
#include <errno.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "bmp.h"


//utility functions
void say(char*, ...);
void error(char*, ...);

int test_bmp();