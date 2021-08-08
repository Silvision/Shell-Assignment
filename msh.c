// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017, 2021 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 7f704d5f-9811-4b91-a918-57c1bb646b70
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

/*
 *
 *  Name: Richard Nguyen
 *  ID: 1001360247
 *
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11     // Mav shell only supports 10 arguments (+ 1 command itself)

int main(){

	// Creation of arrays to use for later purposes
	// historyArray to store previous commands (requirement)
	// pid_list to store list of past pids (requirement)
	// h and p are going to be used as iterators.
	char * historyArray[15];
	pid_t pid_list[15];
	char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
	int h = 0, p = 0;
	

	while( 1 ){
	// Print out the msh prompt
	printf ("msh> ");

	// Read the command from the commandline.  The
	// maximum command that will be read is MAX_COMMAND_SIZE
	// This while command will wait here until the user
	// inputs something since fgets returns NULL when there
	// is no input
	while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

	// Check if the first spot from the user input has an ! in it.
	// This is for the history requirement. It's here so it could overwrite the cmd_str
	// before it gets to doing anything else.
	// It takes away the ! by atoi'ing the string by moving the pointer forward by 1 with
	// (cmd_str++;) atoi then changes it to an int.
	// It then checks if the number is even there in the historyArray list
	// because if it's not there... its null so it goes to else and prints not there
	// If it is in there then replace the cmd_str and continue on 
	if(cmd_str[0] == '!'){
		cmd_str++;
		int commandNum = atoi(cmd_str);
		if(historyArray[commandNum-1] != NULL){
			strncpy(cmd_str,historyArray[commandNum-1],MAX_COMMAND_SIZE);
		}
		else{
			printf("Command not in history.\n");
			continue;
		}
	}

	// Case sensitive condition, checks every spot in the user input and makes it
	// lower case
	for(int i = 0; cmd_str[i]; i++){
		cmd_str[i] = tolower(cmd_str[i]);
	}
	
	/* Parse input */
	char *token[MAX_NUM_ARGUMENTS];

	int   token_count = 0;                                 
                                                           
	// Pointer to point to the token
	// parsed by strsep
	char *argument_ptr;                                         
                                                           
	char *working_str  = strdup( cmd_str );                

	// we are going to move the working_str pointer so
	// keep track of its original value so we can deallocate
	// the correct amount at the end
	char *working_root = working_str;

	// Tokenize the input strings with whitespace used as the delimiter
	while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
		(token_count<MAX_NUM_ARGUMENTS)){
	token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
	if( strlen( token[token_count] ) == 0 ){
		token[token_count] = NULL;
	}
	
	token_count++;
	}


	// Empty input requirement. 
	// Everytime they just press enter without anything,
	// continue will take it back to the while loop, where msh> is printed again.
	if(token[0] == NULL){
		continue;
	}

	// Quit / Exit requirements. (quit or exit the shell)
	// It compares the tokenized string's first spot / word.
	// If it is equal to either quit or exit, then the process will exit it.
	if((strcmp(token[0], "quit") == 0) || (strcmp(token[0], "exit") == 0)){
		exit(0);
	}
	
	// History requirement (show last 15 commands)
	// Controls last 15 commands and stores it into the historyArray. h++ is used
	// as an iterator for the next command to put into the next spot.
	// When it is over 15, we move all the contents of historyArray up by 1
	// so that the very oldest value at spot 0 is gone.
	// historyArray[14] is to remember to store the latest command because
	// at that point h = 15, so it doesn't go into the if statement to store it.
	if(h < 15){ 

		historyArray[h] = strndup(cmd_str, MAX_COMMAND_SIZE);
		h++;

	}
	else{	
		for(int i = 0; i < 15; i++){
			historyArray[i] = historyArray[i+1];
		}
		historyArray[14] = strndup(cmd_str, MAX_COMMAND_SIZE);	
	}

	//If history command is called, simply print the history array.
	// Uses for loop to iterate through the array and print it.
	// i < h condition is so that it only prints how many commands have been
	// put in / stored. With the nature of the h<15 above, this will only
	// print 15 commands.
	// continue is so that the program resets and prepares msh> again for next input
	if(strcmp(token[0], "history") == 0){
		for(int i = 0; i < h; i++){
			printf("[%d] %s",i+1,historyArray[i]);
		}
		continue;
	}
 
	// Listpids requirement (give list of recently made pids, up to 15).
	// Like other strcmp, it compares the first tokenized spot to see if it is listpids
	// if it is, it prints the whole array using a for loop with some formatting included
	// continue so it prepares for next command back at top (msh>   )
	if(strcmp(token[0], "listpids") == 0){
		if(pid_list[0] == 0){
			printf("No pids have been created yet.\n");
		}
		for(int i = 0; i < p; i++){
			printf("[%d] %d\n",i+1,pid_list[i]);
		}
		continue;
	}

	// cd requirement (changes directory to given path)
	// compares if it calls cd and then it utilizes the chdir function.
	// Since token[0] is the cd itself, or the command itself, we'd have to look at
	// token[1] to see what the actual path the user put in wants us to go to.
	// of course continue to put the process back at the top for next commands
	if(strcmp(token[0],"cd") == 0){
		chdir(token[1]);
		continue;
	}

	// Fork 
	// Fork, and if the returned value is 0, that means it is the child. Go into the
	// child and run the execvp command. If a valid command is not given, return
	// command is not found along with what they tried to input.
	pid_t pid = fork();
	if(pid == 0){
		
		int ret = execvp(token[0],token);
		if( ret == -1){
			printf("%s: Command not found\n",token[0]);
			exit(0);
		}

	}
	// In the parent, wait til the execvp child is done. 
	// Here we keep track of the pids. The child will return a pid if a
	// proper command was given. Store said pid into the list and then use p as
	// the iterator to store more. Just like we did with the history array
	// move the pids up by 1 if there are more than 15 to remove the oldest pid.
	else{
		int status;
		wait( &status);
		if(p < 15){
			pid_list[p] = pid;
			p++;
		}
		else{	
			for(int i = 0; i < 15; i++){
				pid_list[i] = pid_list[i+1];
			}
			pid_list[14] = pid;
		}
	}

	free( working_root );

	}
	return 0;
}
