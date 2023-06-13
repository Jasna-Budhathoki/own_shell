//Creating your own shell

#define _GNU_SOURCE // Allows get_current_dir_name()
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <limits.h>
#include <ctype.h>

#define BUFSIZE 2000

// global variables

//environment global variable
extern char **environ;

//creating an environment variable "MHCPROMPT"
char *envvar = "MHCPROMPT";

// global variables for pid
pid_t pid;

//global variable for status
int status;

//environment variable to store exit status code
char *envvar2 = "?";

/*
* pwd command function that prints out the directory. Takes in command and command length as arguments and returns a boolean
*/
bool cmd_pwd (char ** cmd, int cmdlen)
{

  if (0 != strcmp(cmd[0], "pwd")) return false;

  if (cmdlen == 1){ //checks if the length of command is 1
    char buffer[PATH_MAX]; // PATH_MAX is 4096. PATH_MAX is the maximum permitted size of file paths supplied via system calls
    printf("%s \n", getcwd(buffer, sizeof(buffer))); //gets the path name of the working directory
  } else {
    fprintf(stderr, "Too many arguments\n"); //pwd takes no argmuments, prints out an error if cmdlen is not 1
  }

  return true;
}

/*
* exit command function that should exit with an exit code of 0 if no arguments are provided.  If it's given an argument, it should be an integer exit code
*/
bool cmd_exit (char ** cmd, int cmdlen)
{
  char * secondArg = cmd[1];
  bool notValid = false;

  if (0 != strcmp(cmd[0], "exit")) return false;

  // too many arguments passed
  if (cmdlen >= 3){
    fprintf(stderr, "exit Error: more than 2 arguments passed. Type exit, or exit with one exit code\n");
    return true;
  }
  // exit command with exit code argument
  if (cmdlen == 2){

    // checking if the exit argument is a number
    for (int i = 0; i<(strlen(secondArg) - 1); i++)
    {
        if (isdigit(secondArg[i]) == 0){
          notValid = true;
          break;
        }
    }

    // if exit argument is not valid (not a number) then print error
    // otherwise exit with the exit code argument
    if (notValid){
      fprintf(stderr, "exit Error: exit argument is invalid. exit argument needs to be a number\n");
      return true;
    } else {
        exit(atoi(cmd[1]));
    }
  // no exit argument is provided

  }

  exit(0);
  return true;
}

/*
* cd command function that changes the working directory to the path stored in the $HOME environment variable.
* when the user enters cd with an argument, shell should change the working directory to the directory specified by the argument.
*/
bool cmd_cd (char ** cmd, int cmdlen)
{
  // If given an argument, it should be a directory; cd to it (or print an
  // error!)
  // If no argument, cd to the home directory (you can assume the HOME
  // environment variable contains the path to it).

  if (0 != strcmp(cmd[0], "cd")) return false;

  //prints out an error message if there are too many arguments

  if (cmdlen > 2){
    fprintf(stderr, "Too many arguments passed\n");
  }else if (cmdlen == 1){
    int homedir = chdir(getenv("HOME")); //takes to the HOME directory
    if (homedir != 0){
      fprintf(stderr, "home directory not found\n"); //prints an error if home directory does not exist
    }
  }else if (cmdlen == 2){
    int currentDir = chdir(cmd[1]);
    if (currentDir !=0)
    {
      fprintf(stderr, "Directory not found\n");
    }

  }
  return true;
}

/*
* set command function that sets an environment variable. It takes two arguments - first is a variable name, and the second is its desired new value
* if the command set is called with no other arguments, the function prints all the environment variables
* if the command set is called with two environment variables, the environment variable contains second environment variable
*/
bool cmd_set (char ** cmd, int cmdlen){

  if (0 != strcmp(cmd[0], "set")) return false;
  //if MHCPROMPT is being set to an environment variable
  //check if the first character is a $
  //get the rest of the string from cmd[2] after $
  //check if it is an environment variable
  //if it is an environment variable, set MHCPROMPT to the environment variable

  if (cmdlen == 3){

      int envVal = setenv(cmd[1], cmd[2], -1);

      if (envVal != 0){
        fprintf(stderr, "set Error: new value is not set in environment\n");
      }

  } else {
    if (cmdlen < 1){
      fprintf(stderr, "set Error: arguments passed are not enough. Please type variable name and desired new value name in that order.\n");
    } else if (cmdlen > 3){
      fprintf(stderr, "set Error: Too many arguments. Please type variable name and desired new value name in that order.\n");
    } else if (cmdlen == 1){ //print the environment variables
      char **environment_var = environ;
      for (; *environment_var; environment_var++) {
        printf("%s\n", *environment_var);
      }
      return 0;
    }
  }

  return true;

}

char * getEnvironment (char * cmd){

  char * env;

  printf("hello");

  env = cmd + 1;

  if(!getenv(env)){
    fprintf(stderr, "The environment variable %s was not found.\n", env);
      exit(1);
  }else{
    // return_environ = getenv(env);
    printf("get_environment %s\n", getenv(env));
    return (getenv(env));

}

}


/*
* sets environment variable $ to exit code status
*/

void set_exit_status (int *status){
    char * status_str = NULL;
    sprintf(status_str, "%d\n", *status);
    setenv("?", status_str,-1);
    printf("$: %s\n", status_str);
}

/*
* try external command function that checks forbackground vs foreground processes
*/
void try_external (char ** cmd, int cmdlen)
{
  // Run an external command.  To do so...
  // 1. Check to see if you should run it in the background
  // 2. fork()
  //    After forking, the child should:
  //    1. exec(); if successful, this never returns, but you should...
  //    2. Check for error (e.g., command not found!)
  //    3. exit()!  You don't want to have the *child* go back to main()!
  //    After forking, the parent should:
  //    1. Maybe wait for the child?
  //    2. return back to main() so we re-enter the prompt loop

  bool background = false;

  //  check if it should be run in the background
  if (0 == strcmp(cmd[cmdlen-1], "&")){
    // set background to true
    background = true;
  }

  // set end of cmd to NULL terminator
  cmd[cmdlen] = NULL;

  // fork
  pid_t pid;
  pid = fork();
  if (pid == 0)
	{
    if (background){
      cmd[cmdlen-1] = NULL;
    }
		execvp(cmd[0], cmd);
		fprintf(stderr, "%s: not found\n", cmd[0]);
    //exec_fail = true;
		exit(0);
	}
  // if fork is unsuccessful
	else if (pid < 0)
	{
    fprintf(stderr, "Fork unsuccessful\n");
    //exec_fail = true;
    exit(0);
	}
  // if it is not background
	else if (!background)
  {

    // create an integer status
    if (waitpid(pid, &status, 0) == -1 ) {
        perror("waitpid failed");
    }

    set_exit_status(&status);
  }
  }

/*
* exec command executes a command without creating a new process
*/

bool cmd_exec (char ** cmd, int cmdlen) {
  // check if command is exec
  if (strcmp(cmd[0], "exec") != 0) {
    return false;
  }
  // if there is no argument, print error
  if (cmdlen < 2){
    fprintf(stderr, "Too little arguments.\n");
    //exec_fail = true;
    return false;
  }
  else {
    // remove 'exec' from command
    cmd = cmd + 1;
    cmdlen --;

    try_external(cmd, cmdlen);
  }
  return true;

}

/*
* reap all finished children function that reaps children by checking if they are comopleted
*
*/

void reap_all_finished_children (void)
{
  // Use waitpid() and one of its flags to try to reap a child.
  // If you succeed, try to reap another.  If that succeeds, try to do it
  // again, etc.!  (You need to do this because more than one child might
  // have terminated.)
  //TODO

  int waitpid_return_val =  waitpid(-1, &status, WNOHANG);

  while (waitpid_return_val == 0){ //checks if there are more children that still need to be reaped

    waitpid_return_val = (waitpid(-1, &status, WNOHANG)); //tries to reap more children. checks with the child processes to see if they need to be reaped

      if (waitpid_return_val == -1 ) {
        perror("waitpid failed");
        exit(EXIT_FAILURE);
      }

      set_exit_status(&status);
  }

  //STORE status into $ in a sep

}

void sighandler (int)
{
  // Do nothing -- we use this to do nothing in response to Ctrl-C
}

int main (int argc, char * argv[])
{
   // This makes it so we ignore if the user types Ctrl-C
  signal(SIGINT, sighandler);

  // getline() allocates memory for the string it reads from standard
  // input.  We keep a pointer to that memory.  It also keeps track of
  // how much memory it allocates.

  char * rawInput = NULL;
  size_t len = 0;
  char * argToken[255];
  int totalCommandCount;

  // Along with some more minor details, a shell more or less...
  // 1. Prints a prompt
  // 2. Reads a line of input
  // 3. Tries to interpret that as a built-in command
  // 4. If it's not a built-in command, tries to execute it as an
  //    external program
  // 5. Repeats until you exit or standard input closes

  // By default, set the MHCPROMPT global variable to "mhcsh"

  setenv("MHCPROMPT", "mhcsh",-1);

  if (argc > 1)
  {
    return 1;
  }

  while (true)
  {
    //printf("mhcsh> ");
    //get the environment variable value for MHCPROMPT

    //if cannot get the environment variable, print an error message
    if(!getenv(envvar)){
      fprintf(stderr, "The environment variable %s was not found.\n", envvar);
      exit(1);
    }

    //print out the value of environment variable MHCPROMPT that is dynamic
    printf("%s> ",getenv(envvar));

    if (getline(&rawInput, &len, stdin) == -1)
    {
      exit(0);
    }

    //PWD variable holds current working directory
    char *dir = get_current_dir_name();
    int pwd = setenv("PWD", dir, -1);
    free(dir);

    if (pwd != 0)
    {
      fprintf(stderr, "Error in setting PWD\n");
    }

    // Remove the newline char, if exists, after using getline()
    size_t rawInputLen = strlen(rawInput) -1;

    if(rawInput[rawInputLen] == '\n')
    {
      rawInput[rawInputLen] = '\0';
    }

    if (rawInputLen == 0)
    {
      continue;
    }
    else
    {
      //extract the first word with token pointing to the first word
      //set totalCommandCount = 0
      char * token = strtok(rawInput, " ");
      totalCommandCount = 0;

      //loop through the string to extract all other tokens
      while (token != NULL)
      {
        argToken[totalCommandCount] = token;
        token = strtok(NULL, " ");
        totalCommandCount++;
      }

      for (int i = 0; i < totalCommandCount; i++) {
        //printf("token is %s\n", argToken[i]);
        char chk[i];
        chk[1] = 0;
        chk[0] = argToken[i][0];
        //printf("first character is %s\n", chk);
        //chk is a string with a null character in the end
        if (0 == strcmp(chk, "$")){
          argToken[i] = getEnvironment(argToken[i]);
        }

      }



      argToken[totalCommandCount] = NULL;

      //for loop that iterates through argtoken
      //checks if any arg starts with a $
      //if yes, take the string that starts after $
      //find the string in the environment list with getenv


      if (cmd_exit(argToken, totalCommandCount)) continue;
      if (cmd_cd(argToken, totalCommandCount)) continue;
      if (cmd_pwd(argToken, totalCommandCount)) continue;
      if (cmd_set(argToken, totalCommandCount)) continue;
      if (cmd_exec(argToken, totalCommandCount)) break;

      // if (!exec_fail) {
      try_external(argToken, totalCommandCount);
      // }
      // exec_fail = false;
      reap_all_finished_children();
  }
 }

  printf("\n");
  return 0;
}
