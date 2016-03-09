#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// lib for executing various directory operations
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
// lib for finding uname data
#include <sys/utsname.h>
#include <unistd.h>
// lib for fetching userid and with that other personal detail
#include <pwd.h>
// Signals
#include <signal.h>

int Var=0,inbuilt=0,new_fg_flag=0;
int bufsize =64;
char Cwd[100];
struct process
{
	char Pname[150];
	int Pid , Pstatus;
};
struct process Process_list[20000];
void  SYScommands(char **argv,int flag,int inredirect_flag,int outredirect_flag,int input,int output);
void Pipe_cmmd(int pipenumber,char **argv,int size,int outdirect_flag,int output,int inredirect_flag,int input);
void Echo_cmmd(char **command,int k);
void UserPinfo(int id);
void Pwd_cmmd();
void CD_cmmd(char **command);
void Overkill_cmdd();
void Jobs_cmmd();
void Check_exited_process();
void Kjob_cmmd(char **command);
void fg_cmdd(char **command);
void Fg_2_bg()
{
	int  stat,i;
	int pid=getpid();
	new_fg_flag = pid;
	// printf("%d\n",pid);
}

int main()
{
	char SCdir[1000];


	struct utsname Uname_data;
	struct passwd *usernm;
	uid_t userid;
	int m,n,k=0,collcount=0,flag=0,input, output,redirect_flag=0, inredirect_flag=0,outredirect_flag=0,fgbg_flag=0,L,G,pipeflag=0,status;

	uname(&Uname_data);

	userid = geteuid ();
	usernm = getpwuid (userid);

	printf("\n");
	printf("------------------------------ Welcome ---------------------------\n");
	printf("\n");
	while(1)
	{
		Check_exited_process();
		/*  Signal to check for 'Ctrl+z' . If the signal is caught function Fg_2_bg is called*/
		signal(SIGTSTP,Fg_2_bg);
		signal(SIGINT, SIG_IGN);

		getcwd(Cwd,100);
		//         Username and hostname //
		printf("<");
		fputs (usernm->pw_name,stdout);
		printf("@%s:",Uname_data.nodename);
		if (strcmp(Cwd,"/home") != 0)
			printf("~");
		if (strcmp(Cwd,"/home/vikas") != 0)
			fputs(Cwd,stdout);
		printf("> ");
		//----------------------------------//

		size_t linesize = 0;
		k= collcount=  inbuilt = flag=0;

		char **command = malloc(bufsize * sizeof(char*));
		char **Nocommd = malloc(bufsize * sizeof(char*));
		char *word,*reword;
		char *line = NULL;

		getline(&line,&linesize,stdin);

		word = strtok(line,";");
		m=0;
		Nocommd[m] = word;
		m++;
		while((word = strtok(NULL,";"))!= NULL)
		{
			Nocommd[m]=word;
			m++;
		}
		
		for (n = 0; n < m; n++)
		{
			pipeflag = G= k =redirect_flag = inredirect_flag = outredirect_flag = fgbg_flag = input = output = 0;

			reword = strtok(Nocommd[n]," \t");
			command[k] = reword;
			k++;

			while((reword = strtok(NULL," \t"))!=NULL)
			{
				command[k]=reword;
				if (command[k][strlen(command[k])-1] == '\n')
					command[k][strlen(command[k])-1] = '\0';
				k++;
			}
			if (command[0][strlen(command[0])-1] == '\n')
				command[0][strlen(command[0])-1] = '\0';

			if (strcmp(command[0],"quit")  == 0)
			{
				inbuilt =1;
				printf(" ------------------  GoodBye! -----------------------\n");
				flag=1;
				break;
			}	

			if(strcmp(command[0],"jobs")  == 0 && redirect_flag == 0)
			{
				inbuilt =1;
				Jobs_cmmd();
			}


			if(strcmp(command[0],"overkill")  == 0 && redirect_flag == 0)
			{
				inbuilt =1;
				Overkill_cmdd();
			}
			if(strcmp(command[0],"kjob")  == 0 && redirect_flag == 0)
			{
				inbuilt=1;
				Kjob_cmmd(command);
			}
			if(strcmp(command[0],"fg")  == 0 && redirect_flag == 0)
			{
				inbuilt=1;				
				fg_cmdd(command);
			}
			


			// Correct this
			// if(strcmp(command[0],"Ran")  == 0 ) 
			// {
			// 	inbuilt=1;
			// 	sy-----*******s-----tem("ps -e | grep gedit");
			// }

			int T=0,rf,rb,P,andf,pipenumber=0,diff=0;
			for ( T = 0; T < k; ++T)
			{
				if (strcmp(command[T], "|")==0)	
				{
					if(command[T+1] != NULL)
					{
						pipeflag =1;
						inbuilt=1;
						pipenumber++;
					}
					else
						printf("Invalid Pipe command\n");

				}

				if (strcmp(command[T], "&")==0)
				{
					andf = T;
					fgbg_flag=1;
				}
				else if(strcmp(command[T],"<") == 0)
				{
					if(command[T+1] != NULL)
					{
						inredirect_flag=1;	
						redirect_flag =1;				
						input = open(command[T+1], O_RDONLY);
						rb = T;
					}
					else
					{
						printf("No such file or directory\n");
						exit(0);
					}
				}
				else if(strcmp(command[T],">") == 0)
				{
					if(command[T+1] != NULL)
					{
						output = open(command[T+1],O_RDONLY | O_WRONLY | O_CREAT , S_IRWXU);
						rf = T;
						outredirect_flag =1;
						redirect_flag =1;
						if(outredirect_flag ==1)
							break;

					}
					else
					{
						printf("No such file or directory\n");
						exit(0);
					}
				}
				else if(strcmp(command[T],">>") == 0)
				{
					if(command[T+1] != NULL)
					{
						output = open(command[T+1], O_CREAT | O_RDWR | O_APPEND,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
						if(inredirect_flag==0)
						{
							rf = T;
						}
						outredirect_flag =1;
						redirect_flag =1;
						if(outredirect_flag ==1)
							break;

					}
					else
					{
						printf("No such file or directory\n");
						exit(0);
					}
				}

			}
			int combineflag=0;
			if(pipeflag==1 && inredirect_flag == 1)
				combineflag=1;
			
			if(fgbg_flag == 1)
				for (P = andf; P < k; P++)
					command[P]=NULL;

			if((outredirect_flag == 1 && inredirect_flag == 0) || (outredirect_flag == 1 && combineflag == 1))
			{
				for (P = rf; P < k; P++)
					command[P]= NULL;
				diff = k-rf;
			}

			if(pipeflag==1 && inredirect_flag == 1)   
				Pipe_cmmd(pipenumber,command,k - diff,outredirect_flag,output,inredirect_flag,input);
			
			if(inredirect_flag == 1)
			{
				for (P = rb; P < k; P++)
					command[P]=NULL;
				diff = k-rb;
			}
			if(pipeflag==1 && inredirect_flag == 0)
				Pipe_cmmd(pipenumber,command,k - diff,outredirect_flag,output,inredirect_flag,input);

			
			if(strcmp(command[0],"pwd") == 0 && redirect_flag == 0 && pipeflag == 0)
			{
				inbuilt =1;
				Pwd_cmmd();
			}

			if(strcmp(command[0],"echo") == 0 && redirect_flag == 0 && pipeflag == 0)
			{
				inbuilt =1;
				Echo_cmmd(command,k);
			}

			if(strcmp(command[0],"cd") == 0 && redirect_flag == 0)
			{
				inbuilt =1;
				CD_cmmd(command);
			}

			if(strcmp(command[0],"pinfo") == 0 && redirect_flag == 0)
			{
				inbuilt =1;
				int prossid;
				if (command[1] == NULL)
					prossid=getpid();
				else
					prossid=atoi(command[1]);
				UserPinfo(prossid);
			}

			else if(strlen(command[0]) != 1 && command[0][strlen(command[0]-1)] != '\0' && inbuilt ==0)
			{
				if(redirect_flag==1)
				{
					SYScommands(command,fgbg_flag,inredirect_flag,outredirect_flag,input,output);
				}
				else if(fgbg_flag==1)
					SYScommands(command,fgbg_flag,inredirect_flag,outredirect_flag,input,output);
				else
					SYScommands(command,fgbg_flag,inredirect_flag,outredirect_flag,input,output);
			}
		}
		if (flag ==1)
			break;
	}
	return 0;
}

/*===========================================*/
/* List of Finctions */
/*===========================================*/

void  SYScommands(char **argv,int flag,int inredirect_flag,int outredirect_flag,int input,int output)
{
	pid_t  pid;
	int  stat;
	pid = fork();
	if (pid  < 0) 
	{   
		printf("*** ERROR ***\n");
		exit(1);
	}
	else if (pid == 0) 
	{
		if(inredirect_flag ==1)
		{
			dup2(input,0);
			close(input);
		}
		if(outredirect_flag==1)
		{
			dup2(output,1);
			close(output);
		}	
		if (execvp(*argv, argv) < 0) 
		{
			printf("*** ERROR: invalid arguments/command ***\n");
			exit(1);
		}
	}
	else 
	{
		if (flag == 0) // Foreground process
		{
			waitpid(-1,&stat,WUNTRACED);// waitpid reports the status of any child processes that have been stopped as well as those that have terminated

			if (WIFSTOPPED(stat)) //Evaluates to a non-zero value if status was returned for a child process that is currently stopped
			{
				// Updating the process & its details like pid,status,name into the list
				Process_list[Var].Pid=pid;
				Process_list[Var].Pstatus=1;
				strcpy(Process_list[Var].Pname,argv[0]);
				Var += 1;
			}
		}
		else  //Background process
		{
			// Updating the process & its details like pid,status,name into the list
			Process_list[Var].Pid = pid;
			Process_list[Var].Pstatus = 1;
			strcpy(Process_list[Var].Pname,argv[0]);
			Var += 1; 
		}
	}
	//printf("Pp: %d with Cp %d\n",getpid(),pid);
}
void UserPinfo(int id)
{
	int i;
	char block1[150],block2[150];
	char Temp[150],KEY[150]={'\0'};
	FILE *fileop;
	// Imformation is fetched from each processes status and exe files.From these files the required fields have been printed out. //

	sprintf(block2,"/proc/%d/exe",id); //file is a symbolic link containing the actual pathname of the executed command.
	sprintf(block1,"/proc/%d/status",id);
	fileop=fopen(block1,"r");
	if (fileop != NULL)
	{
		printf("pid = %d\n",id);// Pid
		for(i=0;i<=11;i++)
		{
			fgets(Temp,sizeof(Temp),fileop);
			if(i==1)
				fputs(Temp,stdout); // State
			else if (i==11)
				fputs(Temp,stdout); // Vm peak
			else if (i==0)
				fputs(Temp,stdout); // NAme
		}
		readlink(block2,KEY,sizeof(KEY));
		printf("Executable Path is : %s\n",KEY); // Path
	}
	else
		printf("ERROR: invalid pinfo argument\n");
}
void Echo_cmmd(char **command,int k)
{
	int n_echo=0,e_echo=0,i; 
	char Postcmmd[100],rePostcmmd[100];

	if (command[1] != NULL)
	{
		for (i = 1; i < k; ++i)
			if(command[i] != NULL)		
			{
				if (strcmp(command[i],"-n") == 0)
					n_echo=1;
				else if (strcmp(command[i],"-e") == 0)
					e_echo=1;
				else if (strcmp(command[i],"-E") == 0)
					e_echo =0;
				else if((strcmp(command[i],"-n") != 0) && (strcmp(command[i],"-e") != 0) && (strcmp(command[i],"-E") != 0))
					strcpy(Postcmmd,command[i]);
			}

		int len,j=-1,foundflag=0;

		// Checking for escape characters.
		for (i = 0; i < strlen(Postcmmd); i ++) 
		{

			if (Postcmmd[i] != '"' && Postcmmd[i] != '\\' && foundflag == 0) 
			{
				j++;
				rePostcmmd[j] = Postcmmd[i];
			}
			else if (Postcmmd[i+1] == '"' && Postcmmd[i] == '\\')
			{
				j++;
				rePostcmmd[j] = '"';
			}
			else if (Postcmmd[i+1] != '"' && Postcmmd[i] == '\\')
			{
				if(e_echo == 0)
				{
					j++;
					rePostcmmd[j] = '\\';
				}
				else if( e_echo ==1 && (Postcmmd[i+1] != 'a' || Postcmmd[i+1] != 'b' || Postcmmd[i+1] != 'e' || Postcmmd[i+1] != 'f'))
				{
					foundflag =1;
				}	
			}

		}
		j++;
		rePostcmmd[j]='\0';
		if (n_echo ==0)
			printf("%s\n",rePostcmmd);
		else
			printf("%s",rePostcmmd);
	}
	else
		printf("\n");

}

void Pipe_cmmd(int pipenumber,char **argv,int size,int outredirect_flag,int output,int inredirect_flag,int input)
{
	int  i,fd[2],intial=0,k=0,n,j;
	pid_t  pid;
	char *pipeword;
	char **Pipecommd = malloc(bufsize * sizeof(char*));
	for (i = 0; i < size; ++i)
	{
		if(strcmp(argv[i],"|") !=0 && argv[i] != NULL && strcmp(argv[i],">") !=0 && strcmp(argv[i],"<") !=0)
		{	
			Pipecommd[k]=argv[i];
			// printf("Pipecommd[%d]: %s\n",k,Pipecommd[k]);
			k++;
			n=k;
		}
		if(strcmp(argv[i],"|") ==0 || i == (size-1))
		{
			k=0;
			// printf("%d\n",i);
			if (pipe(fd) < 0)
				perror("pipe error");

			pid=fork();
			if (pid  < 0) 
				perror("fork error");

			else if(pid == 0)
			{
				dup2(intial,0);
				if(argv[i+1]!=NULL)
					dup2(fd[1],1);
				else if(outredirect_flag ==1 && i == (size-1) ) /* i/o redirection + pipes redirection*/
				{
					dup2(fd[1],1);
					dup2(output,1);
					close(output);
				}
				close(fd[0]);
				if(strcmp(Pipecommd[k],"echo") == 0)
					Echo_cmmd(Pipecommd,n);
				if(strcmp(Pipecommd[k],"pwd") == 0)
					Pwd_cmmd();
				else
					if (execvp(Pipecommd[k],Pipecommd) < 0) 
						perror("pipe error");
				_exit(0);
			}
			else
			{
				wait(NULL);
				// if(outredirect_flag != 1)
				close(fd[1]);
				intial = fd[0];
			}
		}
		if(k==0)
			for (j = 0; j < n; ++j)
				Pipecommd[j] = NULL;

	}
	// exit(0);
}
void Pwd_cmmd()
{
	puts(Cwd);
}
void CD_cmmd(char **command)
{
	if(command[1] == NULL)
		chdir(getenv("HOME"));// Home directory is default "Home" directory

	else 
	{
		if (command[1][strlen(command[1])-1] == '\n')
			command[1][strlen(command[1])-1] = '\0';

		if(chdir(command[1]) != 0 )
			printf("Error in chdir.Directory with such a name does not exits \n");

		else 
		{
			chdir(command[1]);
			//getcwd(Cwd,100);
			//puts(Cwd);
		}
	}
}
void Overkill_cmdd()
{
	int L;
	for(L = 0; L < Var; L++)
	{
		int Pi_d = Process_list[L].Pid;
		if(Process_list[L].Pstatus != 0)
		{
			if (kill(Pi_d, SIGKILL) < 0)
				perror ("kill (SIGKILL)");
			else
				Process_list[L].Pstatus = 0;
		}	
	}
}
void Jobs_cmmd()
{
	int L,ind=0;
	for(L = 0; L < Var; L++)
	{
		if(Process_list[L].Pstatus != 0)
		{
			ind++;
			printf("[%d] Process : %s    pid : [%d]\n",ind,Process_list[L].Pname,Process_list[L].Pid);
		}
	}
}
void Check_exited_process()
{
	int status,m;
	int Pcheck = waitpid(-1,&status,WNOHANG);// waitpid returns immediately instead of waiting, if there is no child process ready to be noticed
	while (Pcheck > 0)
	{
		for (m = 0; m < Var; m++)
			if (Process_list[m].Pid == Pcheck && Process_list[m].Pstatus) // Checking if any of the processes in the list has exited.
			{
				printf("%s with pid %d exited normally\n",Process_list[m].Pname,Process_list[m].Pid);
				Process_list[m].Pstatus = 0;
			}
		Pcheck = waitpid(-1,&status,WNOHANG);
	}
}
void Kjob_cmmd(char **command)
{
	int count=0,tempcmmd,L;
				if(command[1] != NULL && command[2] != NULL)
				{
					for(L = 0; L < Var; L++)
						if(Process_list[L].Pstatus != 0)
						{
							count++;
							tempcmmd = atoi(command[1]);	
							if(tempcmmd == count)
								break;
						}
					if(strcmp(command[2],"1")  == 0)
					{
						if (kill(Process_list[L].Pid, SIGHUP) < 0)
							perror ("kill (SIGHUP)");
						else
							Process_list[L].Pstatus = 0;

					}
					if(strcmp(command[2],"2")  == 0)
					{
						if (kill(Process_list[L].Pid, SIGINT) < 0)
							perror ("kill (SIGINT)");
					}
					if(strcmp(command[2],"3")  == 0)
					{
						if (kill(Process_list[L].Pid, SIGQUIT) < 0)
							perror ("kill (SIGQUIT)");
					}
					if(strcmp(command[2],"4")  == 0)
					{
						if (kill(Process_list[L].Pid, SIGILL) < 0)
							perror ("kill (SIGILL)");
					}
					if(strcmp(command[2],"9")  == 0)
					{
						if (kill(Process_list[L].Pid, SIGKILL) < 0)
							perror ("kill (SIGKILL)");
						else
							Process_list[L].Pstatus = 0;
					}
					if(strcmp(command[2],"20")  == 0 || strcmp(command[2],"17")  == 0 || strcmp(command[2],"18")  == 0)
					{
						if (kill(Process_list[L].Pid, SIGCHLD) < 0)
							perror ("kill (SIGCHLD)");
					}
					if(strcmp(command[2],"19")  == 0 || strcmp(command[2],"25")  == 0)
					{
						if (kill(Process_list[L].Pid, SIGCONT) < 0)
							perror ("kill (SIGCONT)");
					}
					if(strcmp(command[2],"23")== 0)
					{
						if (kill(Process_list[L].Pid, SIGSTOP) < 0)
							perror ("kill (SIGSTOP)");
					}
				}
				else
					printf("Format: kjob <jobNumber> <signalNumber>\n");
}
void fg_cmdd(char **command)
{
	int count=0,L,tempcmmd;
	for(L = 0; L < Var; L++)
		if(Process_list[L].Pstatus != 0)
		{
			count++;
			tempcmmd = atoi(command[1]);	
			if(tempcmmd == count)
				break;
		}

		int p_id=Process_list[L].Pid;
		setpgid(p_id,getpgrp());
		Process_list[L].Pstatus =1;
		waitpid(-1,NULL,WUNTRACED);
}