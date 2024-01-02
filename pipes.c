#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {
	char command[50];
	char* dir_name = argc > 1? argv[1] : "";
	sprintf(command, "ls %s -1 | wc -l",dir_name);//I have chosen to use sprintf with command as a buffer, instead of doing another malloc and strcpy
	FILE *pipe = popen(command, "r");
	
	char buffer[128];//reasonable to assume less than a 128 digits number of students
	//read the output of wc -l
	if(fgets(buffer,sizeof(buffer),pipe)==NULL){
		printf("failed to get number of students");
		pclose(pipe);
		exit(EXIT_FAILURE);
	}
	int num_of_students = atoi(buffer);
	printf("\nNumber of students: %d\n",num_of_students);
	char** students_names = (char**)malloc(sizeof(char*)*num_of_students);
	//close the pipe
	if(pclose(pipe)==-1){
		perror("pclose");
		exit(EXIT_FAILURE);
	}
	return 0;
}
