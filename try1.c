#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
	char* name;
	int grade;
}Student;

int main(int argc, char* argv[]) {
	char command[50];
	char* dir_name = argc > 1? argv[1] : "";
	sprintf(command, "ls %s -1 | wc -l",dir_name);//I have chosen to use sprintf with command as a buffer, instead of doing another malloc and strcpy
	printf("command is: \n%s\n",command);
	FILE *pipe = popen(command, "r");

	char buffer[128];//reasonable to assume less than a 128 digits number of students
	//read the output of wc -l
	if(fgets(buffer,sizeof(buffer),pipe)==NULL){
		printf("failed to get number of students");
		pclose(pipe);
		exit(EXIT_FAILURE);
	}
	printf("buffer is:\n %s\n",buffer);
	int num_of_students = atoi(buffer);
	printf("\nNumber of students: %d\n",num_of_students);
	if(pclose(pipe)==-1){
		perror("pclose");
		exit(EXIT_FAILURE);
	}

	/*Now, to allocate space for the Students*/
	Student** students = (Student**)malloc(sizeof(Student*)*num_of_students);
	for(int j=0;j<num_of_students;j++){
		Student* stud_mem = (Student*)malloc(sizeof(Student));
		students[j] = stud_mem;
	}
	char command2[100];
	sprintf(command2,"ls %s -1",dir_name);
	FILE *pipe2 = popen(command2,"r");
	char names_buffer[50];
	int i = 0;
	while(fgets(names_buffer,sizeof(names_buffer),pipe2)!=NULL){
		//printf("\n Student name is %s",names_buffer);
		char* name_mem_aloc = (char*)malloc(sizeof(char)*(strlen(names_buffer)+1));
		strcpy(name_mem_aloc,names_buffer);
		students[i]->name = name_mem_aloc;
		printf("\nstudents[%d]->name = %s",i,students[i]->name);
		i++;
	}
	if(pclose(pipe2)==-1){
		perror("pclose");
		exit(EXIT_FAILURE);
	}
	/*freeing dynamic mem*/
	for(int k =0 ;k<num_of_students;k++){
		free(students[k]->name);
		free(students[k]);
	}
	free(students);

	return 0;
}
