# OS-LAB1

*******************************************************
*  Name      :  Yuzhe Lu       
*  Student ID:  107626065
*  Class     :  CS 3453 Operating System Concepts          
*  HW#       :  Lab1               
*  Due Date  :  Sept 26, 2018
*******************************************************


                 Read Me


*******************************************************
*  Description of the program
*******************************************************

The program LAB 1 will run a unix based shell program

*******************************************************
*  Source files
*******************************************************

Name:  main.cpp
	Main program.  This is the driver program that will run the main proccedures 
  
Name:  Functions.h
   Contains the declarations

Name: Functions.cpp
   Contains the definitions for Lab1

   
*******************************************************
*  Circumstances of programs
*******************************************************

   The program runs successfully.  
   
   The program was developed and tested on gnu g++ 4.4.2.  It was 
   compiled, run, and tested on csegrid.


*******************************************************
*  How to build and run the program
*******************************************************

1. Uncompress the homework.  The homework file is compressed.  
   To uncompress it use the following commands 
       % unzip [LAB1]

   Now you should see a directory named homework with the files:
        main.cpp
	Functions.h
	Functions.cpp
  makefile
  readme.txt
	

2. Build the program.

    Change to the directory that contains the file by:
    % cd [LAB1] 

    Compile the program by:
    % make

3. Run the program by:
   % ./Lab1

4. Delete the obj files, executables, and core dump by
   %./make clean
