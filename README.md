# About the project 

The shell is a program where the user enters commands (usually by keyboard); these are parsed and executed.  It's a shell that generally runs first in the Terminal application on both the Mac and Linux (and in the Windows PowerShell application).  There are actually many different shell programs available, with bash being a very popular one. 

## Implementation of bash 

In the following project, I am implementing several features of bash. They are as follows: 

* echo is a command (or a program - more on this in a moment) that just prints its command line arguments to standard output.

* Built-in Commands
Whenever the shell accepts a command, it should check whether the command is a built-in command or not. If it is, the shell will invoke my implementation of the built-in command.
Most Unix shells have many built-in commands such as cd, echo, pwd, etc. In this project, I implement four built-in commands: exit, pwd, cd, and set.
* exit
The exit command should cause mhcsh to terminate.  To implement the exit built-in command, I call C’s exit function.  If not given any arguments, it should exit with an exit code of 0.  If it's given an argument, it should be an integer exit code (e.g., "exit 3").
* pwd
This command Prints the Working Directory.  That is, it shows you what directory you're in.  When a user types pwd, I simply call C’s getcwd function (or one of the other variants), and show the result.  The pwd command has no arguments.  This has a connection to the implementation of the PWD variable, but is a separate thing!
* cd
When the user enters cd (without arguments), the shell should change the working directory to the path stored in the $HOME environment variable. I use the call getenv("HOME") in my source code to obtain this value.  To actually change directories, I use the chdir function.  
* set
The set command sets an environment variable, and it should take two arguments.  The first is a variable name, and the second is its desired new value. The external program env should list all its environment variables.  These are copied to new processes by fork(). 

# Other features being implemented

* Support variable substitution: 

Save exit code in “$?”:

The exit code of the most recent foreground program should be put into the "$?" variable (similar to what bash does).  "$?" should be set to reflect failures in built-in commands as well.

* Customizable prompt:

Customizable prompt based on value in MHCPROMPT environment variable.  The prompt can be changed to a static string using an environment variable but we can also make it dynamic. 

