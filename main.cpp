#include <stdio.h>

#include <string.h> /* String */
#include <iostream>

#include <sys/types.h> /* Fork */
#include <unistd.h>

#include <sys/wait.h> /* Wait */

#include <stdlib.h> /* Error */

#define EXIT 0x01
#define NORMAL_PROCCESS 0x02
#define BACKGROUND 0x03

using namespace std;

int parseCommand(string input , char **args){
    int argPos = 0;
    int i = 0;
    string temp ;

    while( i < input.size() ){
        temp ="";

        while(i < input.size()  && (input[i] == ' ' || input[i] == '\t') )i++;
        while(i < input.size()  && input[i] != ' ')
            temp+= input[i++];

        if( temp== "exit" || temp == "EXIT")
            return EXIT;

        strcpy(*args++, temp.c_str());
        argPos++;
    }

    args[argPos] = NULL;

    char c = args[argPos-1][sizeof(args[argPos-1])-2];
    if(  c == '&')
        return BACKGROUND;
    else
        return NORMAL_PROCCESS;

}
/**
*   Method to start new process and execute command via execvp().
**/
void execute(char *args[]){
    int status = 0;
    int pid ; ;
    if((pid = fork() ) < 0 ){
        printf("***ERROR: error in process #%i",pid);
        exit(EXIT_FAILURE);
    }else if( pid == 0 ){
        execvp(args[0],args);
    }
}

/**
* Wait until the child process terminated.
**/
void wait_process(){
    int status;
    if( wait(&status) == -1 ){
         perror("***Error:");
    }else{
       if (WIFSIGNALED(status) != 0)
          printf("Child process ended because of signal %d.n",
                  WTERMSIG(status));
       else if (WIFEXITED(status) != 0)
          printf("Child process ended normally; status = %d.n",
                  WEXITSTATUS(status));
       else
          printf("Child process did not end normally.n");
    }
}
int  main(void){

    while(1){
        printf("%s:~$ ",get_current_dir_name());

        string input;
        getline(std::cin,input);

        char* args[48];
        int result = parseCommand(input , args);

        switch(result){
            case BACKGROUND:
                execute(args);
                break;
            case NORMAL_PROCCESS:
                execute(args);
                wait_process();
                break;
            case EXIT:
                char *arr[] = {"exit",NULL};
                execvp(arr[0],arr);
                break;
        }
    }

   return 0;
}
