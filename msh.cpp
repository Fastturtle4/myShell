//Paul Barrett
//Project 4, my shell
//CS270

#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sstream>
#define VARMAX 255
#define REASONABLEAMT 150
using namespace std;

int main()
{
vector<string> var_names;		//names created vars
vector<string> var_vals;		//values corresponding to var names, in same order
vector<int> procids;
vector<string> procnames;
string promptname = "msh > ";		//display initial prompt
string invalidarg = "Error: Invalid number of arguments.";	//error to print if invalid # of args
bool showtok = false;

//LOOP SHELL INTERFACE
do
{
	//DISPLAY PROMPT, STORE INPUT, USEFUL VARS
	cout << promptname;			//display prompt
	char command[VARMAX];			//create string for command
	cin.getline(command, VARMAX);		//read command
	if(cin.eof())				//close on ctrl-d
	{
		cout << endl;
		exit(1);
	}
	vector<char*> tokens;			//vector of tokens
	char* tok_val;				//value for receiving tokens
	tok_val = strtok(command, " ");		//get first token
	int numargs = -1;			//keep track of # of args
	
	//FILL TOKEN VECTOR - SCANNER
	while(tok_val != NULL)
	{
		tokens.push_back(tok_val);	//add to token vector
		tok_val = strtok(NULL, " ");	//get next token
	}
	
	//PARSER
	int begin = -1;
	int toksize = tokens.size();
	for(int i = 0; i < toksize; i++)
	{
		//check if there is a comment and remove rest of line from operable input
		if(tokens[i][0] == '#')
		{			
			begin = i;	
		}
		if (begin != -1)
		{
			tokens.pop_back();
		}

		//replace variable names with corresponding value
		if(tokens[i][0] == '^')
		{
			char* whichvar = strtok(tokens[i], "^");
			string whichvarstring = whichvar;
			for(unsigned int g = 0; g < var_names.size(); g++)
			{
				if(var_names[g].compare(whichvarstring) == 0)
				{
					char* varval = const_cast<char*>(var_vals[g].data());
					tokens[i] = varval;
				}
			}
		}
	}
	numargs = tokens.size();	//keep track of number of arguments

	//showtoken set to 1
	if (showtok == true)
	{
		for(unsigned int r = 0; r < tokens.size(); r++)
		{
			cout << "Token[" << r << "] = " << tokens[r] << endl;
		}
	}

	//HANDLE COMMANDS
	if(strcmp(tokens[0], "setvar") == 0)		 //setvar command
	{
		//push name and value into parallel vectors, parser handles matching them
		if(numargs == 3)
		{
			if((strcmp(tokens[1], "ShowTokens") == 0) && (tokens[2][0]-'0' == 1))
			{
				showtok = true;
			}
			var_names.push_back(tokens[1]);
			var_vals.push_back(tokens[2]);
		}
		else 
		{
			cout << invalidarg << endl;
		}
	}
	else if (strcmp(tokens[0], "setprompt") == 0)    //setprompt command
        {
		if(numargs == 2)
		{
			promptname = tokens[1];
		}
		else
                {
                        cout << invalidarg << endl;
                }
        }
	else if (strcmp(tokens[0], "setdir") == 0)    	 //setdir command
        {
		if(numargs == 2)
		{
			chdir(tokens[1]);
		}
		else
                {
                        cout << invalidarg << endl;
                }
		//check if it successfully switched to desired directory
		char currdir[REASONABLEAMT];
		getcwd(currdir, REASONABLEAMT);
		if(strcmp(tokens[1], currdir) != 0)
		{
			cout << "No such directory!\n";
		}
        }
	else if (strcmp(tokens[0], "showprocs") == 0)    //showprocs command
        {
		int shproc;
		waitpid(-1, &shproc, WNOHANG);
		cout << "Background processes: " << endl;
		for(unsigned int x = 0; x < procids.size(); x++)
		{
			if(procids[x]==shproc)
			{
				cout << procnames[x] << endl;
			}
		}
			
        }
	else if (strcmp(tokens[0], "done") == 0)   	 //done command
        {
		if(numargs == 2)
		{
			exit(tokens[1][0]-'0');
		}
		else if(numargs == 1)
		{
			exit(0);
		}
		else
		{
			cout << invalidarg << endl;
		}
        }
	else if (strcmp(tokens[0], "run") == 0)    	 //run command
        {
		char* runcommands[tokens.size()];
		copy(tokens.begin()+1, tokens.end(), runcommands);
		runcommands[tokens.size()-1] = NULL;
		pid_t pid = fork();

		if(numargs >= 2)
		{
			if (pid == 0) //child process
			{
				execvp(runcommands[0], runcommands);
				cout << "The execution failed with the following error: " <<  strerror(errno) << endl;	
				exit(1); //shouldn't get here on a valid exec
			} 
			else if (pid > 0) 
			{
				int status;
				waitpid(pid, &status, 0);
			}
			else if (pid < 0)
			{
				cout << "There was a problem forking" << endl;
			}	
		}
		else
		{
			cout << invalidarg << endl;
		}
        }
	else if (strcmp(tokens[0], "fly") == 0)    	 //fly command
        {
		char* flycommands[tokens.size()];
                copy(tokens.begin()+1, tokens.end(), flycommands);
                flycommands[tokens.size()-1] = NULL;
                pid_t pid = fork();
		if(numargs >= 2)
		{
	                if (pid == 0)
	                {
	                        execvp(flycommands[0], flycommands);
	                        cout << "The execution failed with the following error: " <<  strerror(errno) << endl;
	                        exit(1);
	                }
	                else if (pid > 0)
	                {
	                        int statusfly;
				procids.push_back(statusfly);
				procnames.push_back(tokens[1]);
	                        waitpid(pid, &statusfly, WNOHANG);
	                }
	                else if (pid < 0)
	                {
	                        cout << "There was a problem forking" << endl;
	                }
		}
		else
		{
			cout << invalidarg << endl;
		}
        }
	else if (strcmp(tokens[0], "tovar") == 0)        //tovar command
        {
		char* tovarcommands[tokens.size()];
                copy(tokens.begin()+2, tokens.end(), tovarcommands);
                tovarcommands[tokens.size()-2] = NULL;
                pid_t pid = fork();
		char file[20] = "/tmp/myfile.txt";
		
		if(numargs >= 3)
		{	
	                if (pid == 0)	//child
	                {
				int fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
				dup2(fd, 1);
				dup2(fd, 2);
				close(fd);
	                        execvp(tovarcommands[0], tovarcommands); 
				cout << "The execution failed with the following error: " <<  strerror(errno) << endl;
	                        exit(1); 
	                }
	                else if (pid > 0)	//parent
	                {
	                        int tovarstatus;
	                        waitpid(pid, &tovarstatus, 0);
	                }
	                else if (pid < 0)
	                {
	                        cout << "There was a problem forking" << endl;
	                }
			char buffer[REASONABLEAMT];
			ifstream infile;
			infile.open("/tmp/myfile.txt");
			infile >> buffer;
			infile.close();
			var_names.push_back(tokens[1]);
			var_vals.push_back(buffer);
			cout << buffer << endl;
			
	        }
		else
		{
			cout << invalidarg << endl;
		}
	}
	else						 //unrecognized/incorrect input
	{
		cout << "Invalid command: " << tokens[0] << endl;
	}
} while(true);	//end loop of shell

return 0;
}
