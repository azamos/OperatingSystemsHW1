#include <stdio.h>
#include <stdlib.h>
int main(int argc, char* argv[]){
	if(argc<3){
		printf("Not enough args were provided. exit...");
		exit(0);
	}
	char* cmp_path = "/usr/bin/cmp";
	pid_t pid = fork();
	if(pid==0){
		printf("FORKED!now highjacking process to run gnome-terninal...");
		char *args[] = {cmp_path,"-s",argv[1],argv[2],NULL};
		int retcode = execvp(cmp_path,args);
		if(retcode==-1){
			perror("exec failed");
			exit(-1);
		}
	}
	else {
		int status;
		waitpid(pid,&status,0);
		if(status==0){
			return 2;
		}
	}
	return 1;
}
