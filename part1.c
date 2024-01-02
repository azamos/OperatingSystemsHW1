#include <stdio.h>
#include <stdlib.h>
int text_cmp(char* filepath1, char* filepath2){
	char* cmp_path = "/usr/bin/cmp";
	pid_t pid = fork();
	if(pid==0){
		char *args[] = {cmp_path,"-s",filepath1,filepath2,NULL};
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

int main(int argc, char* argv[]){
	return text_cmp(argv[1],argv[2]);
}
