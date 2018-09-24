//
//  main.cpp
//  C++Shell
//
//  Created by Yuzhe Lu on 9/20/18.
//  Copyright © 2018 Yuzhe Lu. All rights reserved.
//

#include <iostream>
#include "Functions.hpp"

int main() {
    // insert code here...
    char *linePtr; //points to input
    char *cmd1[MAX_ARGS], *cmd2[MAX_ARGS];  // for pipe and redirects
    char **tokens;
    PipeRedirect pipeRedirect;
    int go = 1;
    
    while(go){
        
        int argc = 0;

        linePtr = readLine();
        
        //parse
        tokens = parseLine(linePtr, argc);
        //check for pipe or redirection
        pipeRedirect = checkPipeRedirect(argc, tokens, cmd1, cmd2);
        if (pipeRedirect == PIPE){
            letsPipeThisShit(cmd1, cmd2);
        }
        
        //check for redirection
        else if (pipeRedirect == REDIRECT_OUT || pipeRedirect == REDIRECT_IN){
            //letsRedirectThisShit(cmd1, cmd2);
            letsRedirectThisShit(pipeRedirect, cmd1, cmd2);
            //ifThisWorksICry(tokens);
        }
        else
        {
            go = execute66(tokens);
        }
        
        //resetti
        for (int i = 0; i < argc; ++i){
            tokens[i] = NULL;
        }

    }
    
    return 0;
}
