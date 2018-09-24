//
//  Functions.cpp
//  C++Shell
//
//  Created by Yuzhe Lu on 9/20/18.
//  Copyright © 2018 Yuzhe Lu. All rights reserved.
//

#include "Functions.hpp"


//for help printout
const char* argumentStrings[] = {
    "cd",
    "exit",
    "help"
};
//built in functions
int(*allowedFunctions[]) (char**){
    &sh_cd,
    &sh_exit,
    &help
};
int numberOfArgumentStrings(){
    return sizeof(argumentStrings)/sizeof(char*);
}



int execute66(char** tokens){
    int i;
    if (tokens[0] == NULL){
        //no commands
        return 1;
    }
    
    for(i = 0; i< numberOfArgumentStrings(); ++i){
        char* argument = tokens[0]; //check first element for command
        //check if it is a builtin command
        if(strcmp(argument, argumentStrings[i]) == 0){
            return (allowedFunctions[i])(tokens);
        }
    }
    
    return launch(tokens);
}

PipeRedirect checkPipeRedirect(int argc, char** tokens, char** cmd1, char** cmd2){
    PipeRedirect result = NONE;
    
    int splitPosition = -1; //position of pipe or redirect
    
    for(int i = 0; i < argc; ++i){
        //check for pipe
        if (strcmp(tokens[i], "|") == 0){
            result = PIPE;
            splitPosition = i;
        }
        //check for redirect_in
        else if (strcmp(tokens[i], "<") == 0){
            result = REDIRECT_IN;
            splitPosition = i;
        }
        else if (strcmp(tokens[i], ">") == 0){
            result = REDIRECT_OUT;
            splitPosition = i;
        }
    }
    //if its a pipe or redirect
    if (result != NONE){
        
        //feed first half of tokens into cmd1
        for(int i = 0; i < splitPosition; ++i){
            cmd1[i] = tokens[i];
        }
        
        int j = 0;
        for(int i = splitPosition + 1; i <argc; ++i){
            cmd2[j] = tokens[i];    //last half of tokens into cmd2
            ++j;    //increment here because we're not iterating j
        }
        
        cmd1[splitPosition] = NULL; //NULL terminate
        cmd2[j] = NULL;
    }
    
    return result;
}

int launch(char** tokens){
    pid_t chd_pid, par_pid;
    int status;
    
    //fork child
    chd_pid = fork();
    
    if(chd_pid == 0){
        //if it returns anything, throw an error. a process never returns from an exec unless error
        if(execvp(tokens[0], tokens )== -1){
            //error
            perror("The following Error occured");
            exit(EXIT_FAILURE);
        }
    }
    else if (chd_pid < 0){
        perror("The following Error occured");
    }
    else{
        //parent process
        do{
            par_pid = waitpid(chd_pid, &status, WUNTRACED);
        }while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

//pipes output of cmd1 and cmd2

void letsPipeThisShit(char** cmd1, char** cmd2){
    int fds[2];
    pipe(fds);
    pid_t pid1, pid2;
    
    //child 1
    if((pid1 = fork()) == 0){
        dup2(fds[1], 1);
        close(fds[0]);
        close(fds[1]);
        execvp(cmd1[0], cmd1);
        perror("execvp failed");
        
        //child 2
    } else if ((pid2 = fork()) == 0){
        dup2(fds[0], 0);
        
        close(fds[1]);
        execvp(cmd2[0], cmd2);
        perror("execvp failed");
    } else{
        //parent
        close(fds[1]);
        
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    }
    
}

//this one works good
void letsRedirectThisShit(PipeRedirect redirect, char** cmd1, char** file){
    int pid;
    
    switch(pid = fork()){
            case 0:
            
                if(redirect == REDIRECT_IN){    // <
                    //opens file as readonly
                    int fd = open(file[0], O_RDONLY);
                    //call dup2 to copy
                    if(dup2(fd, STDIN_FILENO) == -1){
                        fprintf(stderr, "dup2 failed");
                    }
                    //close out
                    close(fd);
                }
                if(redirect == REDIRECT_OUT){
                    int fd2;
                    if((fd2 = open(file[0], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0){
                        perror("couldnt open output file");
                        exit(0);
                    }
                    dup2(fd2, STDOUT_FILENO);
                    close(fd2);
                }
                
                execvp(cmd1[0], cmd1);
                signal(SIGINT, SIG_DFL);
                perror("error");
                exit(1);
            break;
            
        case -1:
            perror("error");
            break;
            
        default:
            waitpid(pid, NULL, 0);
            
    }
}

//test. not used
void letsRedirectThisShit(char** cmd, char** file){
    int fds[2];
    int fd;
    int count;
    char c;
    pipe(fds);
    pid_t pid1, pid2;
    
    if((pid1 = fork()) == 0){
        fd = open(file[0], O_RDWR | O_CREAT);
        
        if(fd < 0){
            printf("Error: %s\n", strerror(errno));
        }
        
        dup2(fds[0], 0);
        close(fds[1]);
        
        while((count = int(read(0, &c, 1)) > 0)){
            write(fd, &c, 1);
        }
        
        execvp(cmd[0], cmd);
        perror("execvp failed");
        
    }
    
    else if ((pid2 = fork()) == 0){
        dup2(fds[1], 1);
        close(fds[0]);
        
        execvp(cmd[0], cmd);
        perror("execvp failed");
    }
    else{
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
        close(fds[0]);
        close(fds[1]);
    }
    
}


char** parseLine(char* line, int &argc){
    
    int index = 0;
    int buffer = 64;
    char** tokens = (char**) malloc(buffer * sizeof(char*));
    if(!tokens){
        fprintf(stderr, "Error: Allocation Error");
    }
    
    char* token = strtok(line, " ");
    while (token != NULL){
        tokens[index] = token;
        ++index;
        
        if(index >= buffer){
            buffer += buffer;
            tokens = (char**)realloc(tokens, buffer * sizeof(char*));
            if(!tokens){
                fprintf(stderr, "Error: Allocation Error");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, " "); //moves to next value
    }
    tokens[index] = NULL;
    argc = index;   //set count of tokens
    
    return tokens;
}


char* readLine(){
    char* line;
    string input;
    
    cout << "$myShell>> ";
    getline(cin, input);
    
    //turn input string to c string
    line = new char[input.length()+1];
    strcpy(line, input.c_str());
    
    return line;
}

//commands
int sh_cd(char** tokens){
    if(tokens[1] == NULL){
        fprintf(stderr, "Argument expexted to \"cd\"\n");
    }
    else{
        if(chdir(tokens[1]) != 0){
            perror("error");
        }
    }
    return 1;
}

int help(char** tokens){
    int i;
    printf("This is a Shell. Built-in commands are as follows\n");
    for(i = 0; i < numberOfArgumentStrings(); ++i){
        printf(" %s\n", argumentStrings[i]);
        
    }
    return 1;
}

int sh_exit(char** tokens){
   return 0;
}

void ifThisWorksICry(char** tokens){
    char* ofile = NULL;
    char* ifile = NULL;
    char* cp;
    //char** args1;
    char** args2 = (char**)malloc(80*sizeof(char*));
    int i = 0 , j = 0, err = 0;
    int pid;
    
    while(1){
        cp = tokens[i++];
        if(cp == NULL)
            break;
        
        switch(*cp){
            case '<':
                if (cp[1] == 0)
                    cp = tokens[i++];
                else
                    ++cp;
                ifile = cp;
                if (cp == NULL)
                    err = 1;
                else
                    if(cp[0] == 0)
                        err = 1;
                break;
                
            case '>':
                if (cp[1] == 0)
                    cp = tokens[i++];
                else
                    ++cp;
                ofile = cp;
                if(cp == NULL)
                    err = 1;
                else
                    if(cp[0] == 0)
                        err = 1;
                break;
            default:
                args2[j++] = cp;
                break;
        }
    }
    args2[j] = NULL;
    
    switch (pid = fork()) {
        case 0:
            if(ifile != NULL){
                int fd = open(ifile, O_RDONLY);
                if(dup2(fd, STDIN_FILENO) == -1){
                    perror("error");
                }
                close(fd);
            }
            if(ofile != NULL){
                int fd2;
                
                if((fd2 = open(ofile, O_WRONLY | O_CREAT, 0644)) < 0){
                    perror("error");
                    exit(0);
                }
                printf("okay");
                dup2(fd2, STDOUT_FILENO);
                close(fd2);
            }
            execvp(args2[0], args2);
            signal(SIGINT, SIG_DFL);
            perror("error");
            exit(1);
            break;
        case -1:
            perror("error");
            break;
            
        default:
            wait(NULL);
            break;
    }

}
