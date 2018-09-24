//
//  Functions.hpp
//  C++Shell
//
//  Created by Yuzhe Lu on 9/20/18.
//  Copyright © 2018 Yuzhe Lu. All rights reserved.
//

#ifndef Functions_hpp
#define Functions_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>
#include <cstring>

using namespace std;

const int MAX_ARGS = 256;
enum PipeRedirect {PIPE, REDIRECT_IN, REDIRECT_OUT, NONE};

PipeRedirect checkPipeRedirect(int, char**, char**, char**);
int execute66(char**);
int launch(char** tokens);
void letsPipeThisShit(char**, char**);

void letsRedirectThisShit(PipeRedirect, char**, char**);
void letsRedirectThisShit(char**, char**);

char** parseLine(char* line, int &argc);

void ifThisWorksICry(char**);

char* readLine();

//shell command declarations
int sh_cd(char**);
int sh_exit(char**);
int help(char**);


#endif /* Functions_hpp */


