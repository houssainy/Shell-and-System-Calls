#include <stdio.h>

#include <string.h> /* String */
#include <iostream>

#include <sys/types.h> /* Fork */
#include <unistd.h>

#include <sys/wait.h> /* Wait */

#include <stdlib.h> /* Error */

#include <ios>
#include <fstream>

#define EXIT 0x01
#define NORMAL_PROCCESS 0x02
#define BACKGROUND 0x03

using namespace std;

FILE *logFile;

int parseCommand(string input , char **args){
    fputs("**LOG: Start Parsing.\n",logFile);

    int i = 0;
    string temp ;

    int argsPos = 0;
    while( i < input.size() ){
        temp ="";

        while(i < input.size()  && (input[i] == ' ' || input[i] == '\t') )i++;
        while(i < input.size()  && input[i] != ' ')
            temp+= input[i++];

        if( temp== "exit" || temp == "EXIT")
            return EXIT;

        if( temp != ""){
            args[argsPos] = new char[temp.size()];
            strncpy(args[argsPos++], temp.c_str() , temp.size());
            //args++;
        }
    }

    args[argsPos] = NULL;

    //args--;
    temp = args[argsPos-1];
    char c = temp[temp.size()-1];

    if(  c == '&'){
        (*args)[temp.size()-1] = '\0';
        return BACKGROUND;
    }else
        return NORMAL_PROCCESS;

}


/**
* Wait until the child process terminated.
**/
void wait_process(int pid){

    string temp = "**LOG: process ";
    temp+= pid;
    temp+= " is waiting.\n";

    fputs("**LOG: Normal child process was terminated.\n",logFile);

    int status;
    if( wait(&status) == -1 ){
        temp = "**ERROR: Error while waiting "+status;
        temp += "\n";

        fputs(temp.c_str() ,logFile);
        perror("***ERROR");
    }else{

        fputs(temp.c_str(),logFile);
       /*if (WIFSIGNALED(status) != 0)
          printf("Child process ended because of signal %d.n\n",
                  WTERMSIG(status));
       else if (WIFEXITED(status) != 0)
          printf("Child process ended normally; status = %d.n\n",
                  WEXITSTATUS(status));
       else
          printf("Child process did not end normally.n\n");*/

    }
}

void signal_handler(int iSignal){
    fputs("Background child process was terminated.\n",logFile);
}

/**
*   Method to start new process and execute command via execvp().
**/
void execute(char *args[],int result){
    int status = 0;
    int pid ;
    string temp;

    if((pid = fork() ) < 0 ){
        temp = "**ERROR: error in process "+pid;
        fputs(temp.c_str(),logFile);

        fclose(logFile);
        exit(EXIT_FAILURE);
    }else if( pid == 0 ){
        temp = "**LOG: start process "+pid;
        fputs(temp.c_str(),logFile);
        if(execvp(args[0],args) < 0)
            printf("**ERROR: Undifined command\n");
    }else{
        if( result == NORMAL_PROCCESS){
            wait_process(pid);
        }else{
            signal(SIGCHLD, signal_handler);
        }
    }
}

int  main(void){
    logFile = fopen("logFile.txt","w");
    fputs ("Log File:\n",logFile);


    while(1){

        printf("%s:~$ ",get_current_dir_name());

        fputs("**LOG: Take input from user.\n",logFile);
        string input;
        getline(std::cin,input);

        string temp = "**LOG: user input is "+input;
        temp+="\n";
        fputs(temp.c_str(),logFile);

        char* args[48];
        int result = parseCommand(input , args);

        if( result == EXIT ){
            fputs("**LOG: Exit",logFile);
            fclose(logFile);
           // char *arr[] = {"exit",NULL};
            //execvp(arr[0],arr);
            exit(0);
        }else
            execute(args,result);


    }

   fclose(logFile);
   return 0;
}
