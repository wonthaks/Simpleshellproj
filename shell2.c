shell.c
 <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;
pid_t pid;

void int_handler(int signum)
{
  printf("\n");
  kill(pid, SIGKILL);
}

void timed_handler(int signum)
{
  printf("\n");
  kill(pid, SIGKILL);
}

int main() {
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
  
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];
    char cwd[PATH_MAX];
    
    while (true) {
        getcwd(cwd, sizeof(cwd));
        do{ 
            // Print the shell prompt.
            printf("Current working dir: %s%s", cwd, prompt);
            fflush(stdout);

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")
        
            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }
 
        }while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        // TODO:
        // 
        
			  // 0. Modify the prompt to print the current working directory
			  
			
        // 1. Tokenize the command line input (split it on whitespace)
        char * arg = strtok(command_line, delimiters);
        
        int i = 0;
        while (arg != NULL) {
          arguments[i] = arg;
          i += 1;
          arg = strtok(NULL, delimiters);
        }

      
        // 2. Implement Built-In Commands
      
        if (strcmp(arguments[0], "exit") == 0) { // handle exit
          break;
        } else if (strcmp(arguments[0], "pwd") == 0) { // handle pwd
          printf("%s\n", cwd);
          continue;
        } else if (strcmp(arguments[0], "cd") == 0) { // handle cd
          chdir(arguments[1]);
          continue;
        } else if (strcmp(arguments[0], "echo") == 0) { // handle echo
          // i from above will be used since it has the number of arguments passed in.
          char echo_output[MAX_COMMAND_LINE_LEN];
          int curr_index = 1;  // start from 1
          while (curr_index < i) {
            if (curr_index > 1) {
              strcat(echo_output, " ");
            }
            if (arguments[curr_index][0] == '$') {
              char* var;
              char* temp = strtok(arguments[curr_index], "$");
              
              var = getenv(temp);
              
              if (var != NULL) {
                strcat(echo_output, var);
              }
            } else {
              strcat(echo_output, arguments[curr_index]);
            }
            curr_index += 1;
          }
          printf("%s\n", echo_output);
          strcpy(echo_output, ""); // clean output again.
          continue;
        } else if (strcmp(arguments[0], "env") == 0) { // handle env
          char** env = environ;
          char* curr_env;
          while (*env != 0) {
            curr_env = *env;
            printf("%s\n", curr_env);
            env += 1;
          }
          continue;
        } else if (strcmp(arguments[0], "setenv") == 0) { // handle setenv
          char* token = strtok(arguments[1], "=");
          char* var_name = token;
          char* val;
          
          while (token != NULL) {
            val = token;
            token = strtok(NULL, "=");
          }
          
          setenv(var_name, val, 1);
        }
      
    
        // 3. Create a child process which will execute the command line input
        pid = fork();
      
        int is_bg_process = 0;
        if ((arguments[1] != NULL) && (strcmp(arguments[1], "&") == 0)) {
            is_bg_process = 1;
            arguments[1] = NULL;
        }
  
  
        if (pid < 0) {
          perror("Fork error!\n");
          exit(1);
        } else if (pid == 0) {
          signal(SIGINT, int_handler);
          
          // For extra credit, > will be implemented.
          int output_trigger = 0;
          char output_name[259];
          
          for (i = 0; arguments[i] != '\0'; i++) {
            if (strcmp(arguments[i], ">") == 0) {
              arguments[i] = NULL;
              strcpy(output_name, arguments[i+1]);
              output_trigger = 1;
              break;
            }
          }
          
          if (output_trigger == 1) {
            int fd;
            if ((fd = open(output_name, O_RDWR|O_CREAT, 0777)) < 0) {
              perror("Error trying to open the file for output.");
              exit(0);
            }
            
            dup2(fd, 1);
            close(fd);
          }
          
          if (execvp(arguments[0], arguments) < 0) {  // If input was not executable.
            perror("Execution failed.\n");
            exit(1);
          }
          exit(0);
        } else {
          signal(SIGINT, int_handler);
          signal(SIGALRM, timed_handler);
          alarm(10);     // Wait 10 seconds.
          
          if (is_bg_process == 1) {
            is_bg_process = 0;
          } else {
            wait(NULL);
          }
        }
  
        // 4. The parent process should wait for the child to complete unless its a background process
      
      
        // Hints (put these into Google):
        // man fork
        // man execvp
        // man wait
        // man strtok
        // man environ
        // man signals
        
        // Extra Credit
        // man dup2
        // man open
        // man pipes
    // This should never be reached.
    }
    return -1;
}