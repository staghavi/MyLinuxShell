//
//  main.c
//  3305_shell
//
//  Created by Sam Taghavi on 2015-02-05.
//  Copyright (c) 2015 samtaghav. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/types.h>
#include <fcntl.h>


//GLOBAL VARIABLES------------------------------------------------------------------------------------
bool active = true;
int length = 0;
char *cmd_one[151];
char *cmd_two[151];
char *cmd_three[151];
char *cmd_four[151];
char cmds_array[4][151] = {};
char *arrayOne[152];
char *arrayTwo[152];
char *arrayThree[152];
char *arrayFour[152];


//----------------------------------------------------------------------------------------------------

//FUNCTIONS-------------------------------------------------------------------------------------------
void history_print(char history_array[][150], int last_n_cmds);
void read_command(char *cmd, char history_array[][150]);
void execute_command();
void onePipe();
void twoPipe();
void threePipe();

//single pipe functions
void execute_one(int *fd);
void execute_two(int *fd);

//two pipe functions
void exec1(int *pipe1);
void exec2(int *pipe1, int *pipe2);
void exec3(int *pipe1, int *pipe2);

//three pipe functions
void exec_one(int *pipe1);
void exec_two(int *pipe1, int *pipe2);
void exec_three(int *pipe2, int *pipe3);
void exec_four(int *pipe2, int *pipe3);

//----------------------------------------------------------------------------------------------------


int main() {
    
    char history_array[150][150] = {};              //array to store the history of commands
    
    char name[256];               //variable holding users name
    
    printf("Welcome, please type your name and press enter twice!\nAfter each command input press enter again to input another command.\n");   //promting user to enter name
    fgets(name, 256, stdin);
    
    size_t ln = strlen(name) - 1;                   //removing newline character from name
    if (name[ln] == '\n') name[ln] = '\0';
    strcpy(history_array[0], name);                 //recording name into history

    
    while(active){
        char cmd[512];
        while ( getchar() != '\n' );                //flushing stdin
        printf("\n%s> ", name);                     //printing promt userName>
        fgets(cmd, 500, stdin);                     //reading user input

        ++length;
        if(length == 150){                          //if history index has reached 150, remove oldest command and
            length = 149;
            for (int i = 1; i < 150; i++) {
                strcpy(history_array[i-1], history_array[i]);
            }
            strcpy(history_array[length], cmd);
        }else{
            strcpy(history_array[length], cmd);      //save cmd into history
        }
        
        if(strstr( cmd, "exit\n" )) {
            active = false;           //trigger boolean active to break while loop and exit shell
            break;
        }

        read_command(cmd, history_array);
    }
    
    return 0;
}

void execute_command(){
    char string[150];                             //making a copy of the command sent
    strcpy(string, cmds_array[0]);
    char* array[152];
    char *token;                                  //pointer for token variable
    token = strtok(string, " ");                  //first word of a single command is the program name, concatinated with /bin/ for file path
    size_t len = strlen(token) - 1;                   //removing newline character from name
    if (token[len] == '\n') token[len] = '\0';
    
    
    char path[151] = "/bin/";
    
    strcat(path, token);
    array[0] = path;

    array[151] = NULL;
    token = strtok(NULL, " ");

    int index = 1;
    size_t ln;
    while(token != NULL){
        if(index > 150) break;
        ln = strlen(token) - 1;
        if (token[ln] == '\n') token[ln] = '\0';

        array[index] = token;
        token = strtok(NULL, " ");
        index++;
    }
    array[index] = NULL;

    pid_t pid = fork();
    if (pid == -1) {
        perror("problem forking");
    }
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
    }
    else
    {
        //child
        execv(array[0], array);
        _exit(EXIT_FAILURE);
    }
    

}

//Prints out the last n commands, MAX of 150.
void history_print(char history_array[][150], int last_n_cmds){
    int length_of_hist = last_n_cmds;
    for(int i = length; i>-1; i--){
        printf("%s\n", history_array[i]);
        length_of_hist--;
        if(length_of_hist <= 0) break;
    }
}
void onePipe(){
    
    //setup
    char command[150];
    strcpy(command, cmds_array[0]);
    char *token;                                      //pointer for token variable
    
    //seperating two commands by pipe delimiter '|' and removing white space---------------
    token = strtok(command, "|");
    size_t len = strlen(token) - 1;
    if (token[len] == ' ') token[len] = '\0';
    
    cmd_one[0] = token;
    
    
    token = strtok(NULL, "\n");
    size_t ln = strlen(token) - 1;
    if (token[ln] == '\n') token[ln] = '\0';
    if (token[0] == ' '){
        int i = 1;
        while (token[i]) {
            token[i-1] = token[i];
            i++;
        }
        token[ln] = '\0';
    }
    cmd_two[0] = token;
    //-------------------------------------------------------
    
    
    //tokenizing first command for arguments
    char stringOne[150];                             //making a copy of the command sent
    strcpy(stringOne, cmd_one[0]);

    token = strtok(stringOne, " ");                  //first word of a single command is the program name
    len = strlen(token) - 1;                         //removing newline character from name
    if (token[len] == '\n') token[len] = '\0';
    
    arrayOne[0] = token;
    

    arrayOne[151] = NULL;
    token = strtok(NULL, " ");
    
    int index = 1;
    while(token != NULL){
        if(index > 150) break;
        ln = strlen(token) - 1;
        if (token[ln] == '\n') token[ln] = '\0';

        arrayOne[index] = token;
       
        
        token = strtok(NULL, " ");
        index++;
    }

    arrayOne[index] = NULL;
    
    //----------------------------------------------
    
    //tokenizing second command for arguments-------
    char stringTwo[150];                             //making a copy of the command sent
    strcpy(stringTwo, cmd_two[0]);
    
    token = strtok(stringTwo, " ");                  //first word of a single command is the program name, concatinated with /bin/ for file path
    len = strlen(token) - 1;                   //removing newline character from name
    if (token[len] == '\n') token[len] = '\0';
    
    arrayTwo[0] = token;

    arrayTwo[151] = NULL;
    token = strtok(NULL, " ");
    index = 1;
    while(token != NULL){
        if(index > 150) break;
        ln = strlen(token) - 1;
        if (token[ln] == '\n') token[ln] = '\0';

        arrayTwo[index] = token;
    
        token = strtok(NULL, " ");
        index++;
    }
    arrayTwo[index] = NULL;
    //------------------------------------------------
    
   
    //executing commands
    int fd[2];
    pipe(fd);
    
    execute_one(fd);

    execute_two(fd);
    
    wait(NULL);
    close(fd[0]);
    close(fd[1]);
    
    return;
}


void execute_one(int *fd){
    pid_t pid;
    if((pid = fork()) == 0){
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        
        execvp(arrayOne[0], arrayOne);
      
        perror("failed execute_one");
        _exit(1);
    }
    
    
}

void execute_two(int *fd){
    pid_t pid;
    if((pid = fork()) == 0){
        close(fd[1]);
        dup2(fd[0], 0);
        close(fd[0]);
        close(fd[1]);
        
        execvp(arrayTwo[0], arrayTwo);
        
        perror("failed execute_two");
        exit(1);
    }
    return;
    
}

void twoPipe(){
    
    //setup
    char command[150];
    strcpy(command, cmds_array[0]);
    char *token;
    
    //seperating the first command by pipe delimiter '|'-----------------------------------------
    token = strtok(command, "|");
    size_t len = strlen(token) - 1;
    if (token[len] == ' ') token[len] = '\0';
    cmd_one[0] = token;
    
    //seperating the second command by pipe delimiter '|' and removing white space at the start--
    token = strtok(NULL, "|");
    //printf("-------------%s-----------\n", token);
    len = strlen(token) - 1;
    if (token[len] == ' ') token[len] = '\0';
    //printf("-------------%s-----------\n", token);
    if (token[0] == ' '){
        int i = 1;
        while (token[i]) {
            token[i-1] = token[i];
            i++;
        }
        //printf("-------------%s-----------\n", token);
        token[len-1] = '\0';
        //printf("-------------%s-----------\n", token);
    }
    
    cmd_two[0] = token;
    
    //seperating the third command by \n delimiter and removing white space at the start----------
    token = strtok(NULL, "\n");
    size_t ln = strlen(token) - 1;
    if (token[ln] == '\n') token[ln] = '\0';
    if (token[0] == ' '){
        int i = 1;
        while (token[i]) {
            token[i-1] = token[i];
            i++;
        }
        token[ln] = '\0';
    }
    cmd_three[0] = token;
    //-------------------------------------------------------
    
    
    //tokenizing first command for arguments
    char stringOne[150];                             //making a copy of the command sent
    strcpy(stringOne, cmd_one[0]);

    token = strtok(stringOne, " ");                  //first word of a single command is the program name
    len = strlen(token) - 1;                         //removing newline character from name
    if (token[len] == '\n') token[len] = '\0';
    
    arrayOne[0] = token;
    
    arrayOne[151] = NULL;
    token = strtok(NULL, " ");
    
    int index = 1;
    while(token != NULL){
        if(index > 150) break;
        ln = strlen(token) - 1;
        if (token[ln] == '\n') token[ln] = '\0';
        arrayOne[index] = token;
        
        
        token = strtok(NULL, " ");
        index++;
    }
    
    arrayOne[index] = NULL;
    
    //----------------------------------------------
    
    //tokenizing second command for arguments-------
    char stringTwo[150];                             //making a copy of the command sent
    strcpy(stringTwo, cmd_two[0]);
    
    token = strtok(stringTwo, " ");                  //first word of a single command is the program name
    len = strlen(token) - 1;                   //removing newline character from name
    if (token[len] == '\n') token[len] = '\0';
    arrayTwo[0] = token;
    arrayTwo[151] = NULL;
    token = strtok(NULL, " ");
    index = 1;
    while(token != NULL){
        if(index > 150) break;
        ln = strlen(token) - 1;
        if (token[ln] == '\n') token[ln] = '\0';
        
        arrayTwo[index] = token;
        
        token = strtok(NULL, " ");
        index++;
    }
    arrayTwo[index] = NULL;
    //------------------------------------------------
    
    //tokenizing the third command--------------------
    char stringThree[150];                             //making a copy of the command sent
    strcpy(stringThree, cmd_three[0]);
    
    token = strtok(stringThree, " ");                  //first word of a single command is the program name
    len = strlen(token) - 1;                         //removing newline character from name
    if (token[len] == '\n') token[len] = '\0';
    
    arrayThree[0] = token;
    arrayThree[151] = NULL;
    token = strtok(NULL, " ");
    
    index = 1;
    while(token != NULL){
        if(index > 150) break;
        ln = strlen(token) - 1;
        if (token[ln] == '\n') token[ln] = '\0';
        arrayThree[index] = token;
        token = strtok(NULL, " ");
        index++;
    }
    
    arrayThree[index] = NULL;
    //------------------------------------------------
    
    //executing commands
    int id;
    int pipe1[2];
    int pipe2[2];
    
    // create pipe1
    if (pipe(pipe1) == -1) {
        perror("error pipe1");
        exit(1);
    }
    
    // fork child 1
    if ((id = fork()) == -1) {
        perror("error fork1");
        exit(1);
    } else if (id == 0) {
        exec1(pipe1);
    }

    
    // create pipe2
    if (pipe(pipe2) == -1) {
        perror("error pipe2");
        exit(1);
    }
    
    // fork child 2
    if ((id = fork()) == -1) {
        perror("error fork2");
        exit(1);
    } else if (id == 0) {
        exec2(pipe1, pipe2);
    }
    
    // closing pipe1, finished using it
    close(pipe1[0]);
    close(pipe1[1]);
    
    
    // fork child 3
    if ((id = fork()) == -1) {
        perror("bad fork3");
        exit(1);
    } else if (id == 0) {
        exec3(pipe1, pipe2);
    }
    
    //closing pipe 2, finished using it
    close(pipe2[0]);
    close(pipe2[1]);
    
    return;
}
void exec1(int *pipe1) {
    
    close(pipe1[0]);
    dup2(pipe1[1], 1);
    close(pipe1[0]);
    close(pipe1[1]);
    
    execvp(arrayOne[0], arrayOne);
    
    perror("child 3 problem executing");
    _exit(1);
}

void exec2(int *pipe1, int *pipe2) {
    close(pipe1[1]);
    dup2(pipe1[0], 0);
    close(pipe1[0]);
    close(pipe1[1]);
    
    close(pipe2[0]);
    dup2(pipe2[1], 1);
    close(pipe2[0]);
    close(pipe2[1]);
    
    execvp(arrayTwo[0], arrayTwo);
    
    perror("child 2 problem executing");
    _exit(1);
}

void exec3(int *pipe1, int *pipe2) {
    
    close(pipe2[1]);
    dup2(pipe2[0], 0);
    close(pipe2[0]);
    close(pipe2[1]);
    
    execvp(arrayThree[0], arrayThree);
    
    perror("child 3 problem executing");
    _exit(1);
}

void threePipe(){
    //setup
    char command[150];
    strcpy(command, cmds_array[0]);
    char *token;
    
    //seperating the first command by pipe delimiter '|'-----------------------------------------
    token = strtok(command, "|");
    size_t len = strlen(token) - 1;
    if (token[len] == ' ') token[len] = '\0';
    cmd_one[0] = token;
    
    //seperating the second command by pipe delimiter '|' and removing white space at the start--
    token = strtok(NULL, "|");

    len = strlen(token) - 1;
    if (token[len] == ' ') token[len] = '\0';

    if (token[0] == ' '){
        int i = 1;
        while (token[i]) {
            token[i-1] = token[i];
            i++;
        }

        token[len-1] = '\0';

    }
    
    cmd_two[0] = token;
    
    //seperating the third command by pipe delimiter '|' and removing white space at the start--
    token = strtok(NULL, "|");

    len = strlen(token) - 1;
    if (token[len] == ' ') token[len] = '\0';

    if (token[0] == ' '){
        int i = 1;
        while (token[i]) {
            token[i-1] = token[i];
            i++;
        }

        token[len-1] = '\0';

    }
    
    cmd_three[0] = token;
    
    //seperating the fourth command by \n delimiter and removing white space at the start----------
    token = strtok(NULL, "\n");
    size_t ln = strlen(token) - 1;
    if (token[ln] == '\n') token[ln] = '\0';
    if (token[0] == ' '){
        int i = 1;
        while (token[i]) {
            token[i-1] = token[i];
            i++;
        }
        token[ln] = '\0';
    }
    cmd_four[0] = token;
    //-------------------------------------------------------
    
    
    //tokenizing first command for arguments
    char stringOne[150];                             //making a copy of the command sent
    strcpy(stringOne, cmd_one[0]);
    
    token = strtok(stringOne, " ");                  //first word of a single command is the program name
    len = strlen(token) - 1;                         //removing newline character from name
    if (token[len] == '\n') token[len] = '\0';
    
    arrayOne[0] = token;
    
    arrayOne[151] = NULL;
    token = strtok(NULL, " ");
    
    int index = 1;
    while(token != NULL){
        if(index > 150) break;
        ln = strlen(token) - 1;
        if (token[ln] == '\n') token[ln] = '\0';
        arrayOne[index] = token;
        
        
        token = strtok(NULL, " ");
        index++;
    }
    
    arrayOne[index] = NULL;
    
    //----------------------------------------------
    
    //tokenizing second command for arguments-------
    char stringTwo[150];                             //making a copy of the command sent
    strcpy(stringTwo, cmd_two[0]);
    
    token = strtok(stringTwo, " ");                  //first word of a single command is the program name
    len = strlen(token) - 1;                   //removing newline character from name
    if (token[len] == '\n') token[len] = '\0';
    arrayTwo[0] = token;
    arrayTwo[151] = NULL;
    token = strtok(NULL, " ");
    index = 1;
    while(token != NULL){
        if(index > 150) break;
        ln = strlen(token) - 1;
        if (token[ln] == '\n') token[ln] = '\0';
        
        arrayTwo[index] = token;
        
        token = strtok(NULL, " ");
        index++;
    }
    arrayTwo[index] = NULL;
    //------------------------------------------------
    
    //tokenizing third command for arguments-------
    char stringThree[150];                             //making a copy of the command sent
    strcpy(stringThree, cmd_three[0]);
    
    token = strtok(stringThree, " ");                  //first word of a single command is the program name
    len = strlen(token) - 1;                   //removing newline character from name
    if (token[len] == '\n') token[len] = '\0';
    arrayThree[0] = token;
    arrayThree[151] = NULL;
    token = strtok(NULL, " ");
    index = 1;
    while(token != NULL){
        if(index > 150) break;
        ln = strlen(token) - 1;
        if (token[ln] == '\n') token[ln] = '\0';
        
        arrayThree[index] = token;
        
        token = strtok(NULL, " ");
        index++;
    }
    arrayThree[index] = NULL;
    //------------------------------------------------
    
    //tokenizing the fourth command--------------------
    char stringFour[150];                             //making a copy of the command sent
    strcpy(stringFour, cmd_four[0]);
    
    token = strtok(stringFour, " ");                  //first word of a single command is the program name
    len = strlen(token) - 1;                         //removing newline character from name
    if (token[len] == '\n') token[len] = '\0';
    
    arrayFour[0] = token;
    arrayFour[151] = NULL;
    token = strtok(NULL, " ");
    
    index = 1;
    while(token != NULL){
        if(index > 150) break;
        ln = strlen(token) - 1;
        if (token[ln] == '\n') token[ln] = '\0';
        arrayFour[index] = token;
        token = strtok(NULL, " ");
        index++;
    }
    
    arrayFour[index] = NULL;
    //------------------------------------------------
    
    
    //executing commands
    int id;
    int pipe1[2];
    int pipe2[2];
    int pipe3[2];
                            /// INPUT --> PIPE1 --> PIPE2 --> PIPE3 --> OUTPUT
    // create pipe1         /// c.p1-fork-c.p2-fork-c.p3-fork-c.p4-fork-close
    if (pipe(pipe1) == -1) {
        perror("error pipe1");
        exit(1);
    }
    
    // fork child 1
    if ((id = fork()) == -1) {
        perror("error fork1");
        exit(1);
    } else if (id == 0) {
        exec_one(pipe1);
    }
    wait(NULL);
    
    // create pipe2
    if (pipe(pipe2) == -1) {
        perror("error pipe2");
        exit(1);
    }
    
    // fork child 2
    if ((id = fork()) == -1) {
        perror("error fork2");
        exit(1);
    } else if (id == 0) {
        exec_two(pipe1, pipe2);
    }
    wait(NULL);
    //closing pipe1
    close(pipe1[0]);
    close(pipe1[1]);
    
    // create pipe3
    if (pipe(pipe3) == -1) {
        perror("error pipe3");
        exit(1);
    }
    
    // fork child 3
    if ((id = fork()) == -1) {
        perror("error fork3");
        exit(1);
    } else if (id == 0) {
        exec_three(pipe2, pipe3);
    }
    wait(NULL);
    //closing pipe2
    close(pipe2[0]);
    close(pipe2[1]);
    
    // fork child 4
    if ((id = fork()) == -1) {
        perror("error fork4");
        exit(1);
    } else if (id == 0) {
        exec_four(pipe2, pipe3);
    }
    wait(NULL);
    
    //closing pipe 3, finished using it
    close(pipe3[0]);
    close(pipe3[1]);
    

    
    return;
}
//input --> pipe 1
void exec_one(int *pipe1){
    close(pipe1[0]);
    dup2(pipe1[1], 1);
    close(pipe1[0]);
    close(pipe1[1]);
    
    execvp(arrayOne[0], arrayOne);
    
    perror("child 1 problem executing");
    _exit(1);
}

//pipe1 --> pipe 2
void exec_two(int *pipe1, int *pipe2){
    close(pipe1[1]);
    dup2(pipe1[0], 0);
    close(pipe1[0]);
    close(pipe1[1]);
    
    close(pipe2[0]);
    dup2(pipe2[1], 1);
    close(pipe2[0]);
    close(pipe2[1]);
    
    execvp(arrayTwo[0], arrayTwo);
    
    perror("child 2 problem executing");
    _exit(1);
}

//pipe2 --> pipe3
void exec_three(int *pipe2, int *pipe3){
    close(pipe2[1]);
    dup2(pipe2[0], 0);
    close(pipe2[0]);
    close(pipe2[1]);
    
    close(pipe3[0]);
    dup2(pipe3[1], 1);
    close(pipe3[0]);
    close(pipe3[1]);
    
    execvp(arrayThree[0], arrayThree);
    
    perror("child 3 problem executing");
    _exit(1);
}

//pipe3 --> output
void exec_four(int *pipe2, int *pipe3){
    close(pipe3[1]);
    dup2(pipe3[0], 0);
    close(pipe3[0]);
    close(pipe3[1]);
    
    execvp(arrayFour[0], arrayFour);
    
    perror("child 4 problem executing");
    _exit(1);
}

//Scans command for history.
void read_command(char *cmd, char history_array[][150]){
    strcpy(cmds_array[0], cmd);
    //Checking if command has pipes, if so, how many pipes are present
    int numPipes = 0;
    char *test;
    test = strchr(cmd, '|');
    if(test){
        numPipes = 1;
        test = strchr(test+1, '|');
        if(test) {
            numPipes = 2;
            test = strchr(test+1, '|');
            if(test){
                numPipes = 3;
            }
            
        }
        if(numPipes == 3){
            threePipe();
            return;
        }
        else if (numPipes == 2){
            twoPipe();
            return;
        }
        else if (numPipes == 1){
            onePipe();
            return;
        }
    }
    
    //Checking if command has I/O redirection, if so how many and in what order.
    test = strchr(cmd, '<');
    char*  testAlt= strchr(cmd, '>');
   
    if(!test && !testAlt){
        execute_command();
        return;
        
    }
    
    //Checking if History command is issued, and how many entries are requested
    if(strstr( cmd, "history" )){
        char *token = strtok(cmd, " ");
        token = strtok(NULL, " ");
        if(token){
            char *ptr;
            long num = strtol(token, &ptr , 10);
            if(num > 150) puts("Maximum history is 150. Last 150 will be shown instead...");
            history_print(history_array, (int)num );
        }else history_print(history_array, 10);
    }
    
    return;
}
