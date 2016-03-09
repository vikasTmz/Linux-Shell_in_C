Part-I and Part-II

This is a shell implemented by Vikas.

Part-I
{
	Home directory for the shell is the default home directory in the system.So if the program is run from another directory, the path of the current directory will be displayed.On 'cd' it will go to the home directory , just like bash.

	Contains built in commands like pwd,echo,cd,pinfo.
	Echo flags like -n,-e,-E have also been implemented.

	** Bonus question pinfo has been implmented.
	Imformation is fetched from each processes status and exe files.From these files the required fields have been printed out. **

	Error cases have been taken care of.

	System commands are run using execvp().
}

Part-II
{
	(1)  Foreground and background processes have been implemented. Exit status message is also displayed once a background process is terminated.
	(2)  Input-output redirection has been implmented , individually and combined.
	(3)  Piping has been implmented. Multiple pipes of any number can be run.
	(4)  i/o redirection + pipe redirection has been implemented.
	(5)  User defined commands :-
	{
		jobs :- prints out the list of all current running background processes.
		kjob 
		fg :- bring a background process to foreground. done by using setpgid.
		overkill:- kill all background processes.
		quit
		Ctrl + z :- used 'SIGTSTP' to catch signal. Then stopped the foreground process and pushed it into background. On typing 'kjob 1 19' , process continues.
	}
}