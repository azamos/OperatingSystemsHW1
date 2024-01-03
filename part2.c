#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define GCC_PATH  "/usr/bin/gcc"
#define PART1 "./comp.out"

typedef struct {
	char* name;
	int grade;
	char* exe_path;
	char* src_path;
	char* out_path;
}Student;

int main(int argc, char* argv[]) {
	/*PART 1: extracting the 3 lines of data from the config file, whom path is specified in argv[1].*/
	if (argc != 2) {
		printf("Usage: %s <file_path>\n", argv[0]);
		return 1; 
	}
	const char *config_path = argv[1];
	FILE *config_file = fopen(config_path, "r");
	if (config_file == NULL) {
		perror("Error opening file");
		return 1; 
	}
	char* lines[3];
	int counter = 0;
	char config_buffer[51]; //changed to 50
	while (fgets(config_buffer, sizeof(config_buffer), config_file) != NULL) {
		//printf("%s", config_buffer); 
		int n = strlen(config_buffer);
		/*Removing whitespaces...*/
		while(n>0 && (config_buffer[n-1]=='\n' || config_buffer[n-1] == '\r' || config_buffer[n-1]==' '|| config_buffer[n-1]=='\t')){
			config_buffer[--n] ='\0';
		}
		char* allocated_space = (char*)malloc(n+1);
		strcpy(allocated_space,config_buffer);
		//allocated_space[n] = '\0';
		lines[counter] = allocated_space;
		//printf("\nlines[%d]=\n%s",counter,lines[counter]);
		counter++;
	}
	fclose(config_file);
	/*if(strcmp(lines[0],"/home/user/Desktop/Hw1Data/subfolders/")!=0){
		printf("DIFFERENT STRINGS");
	}*/

	/*PART 2: extracting the amount of subfolders and their names, to prepare associated Student instances.*/


	char command[384];
	memset(command,0,384);
	char* dir_name = argc > 1? lines[0] : "";
	//printf("\n\n%s",dir_name);
	sprintf(command, "ls %s -1 | wc -l",dir_name);//I have chosen to use sprintf with command as a buffer, instead of doing another malloc and strcpy
	//printf("\n\ncommand is: \n%s\n",command);
	FILE *pipe = popen(command, "r");

	char buffer[128];//reasonable to assume less than a 128 digits number of students
	//read the output of wc -l
	if(fgets(buffer,sizeof(buffer),pipe)==NULL){
		printf("failed to get number of students");
		pclose(pipe);
		exit(EXIT_FAILURE);
	}
	//printf("buffer is:\n %s\n",buffer);
	int num_of_students = atoi(buffer);
	//printf("\nNumber of students: %d\n",num_of_students);
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
	int index = 0;
	while(fgets(names_buffer,sizeof(names_buffer),pipe2)!=NULL){
		//printf("\n Student name is %s",names_buffer);
		int n = strlen(names_buffer);
		while(n>0 && (names_buffer[n-1]=='\n' || names_buffer[n-1] == '\r' || names_buffer[n-1]==' '|| names_buffer[n-1]=='\t')){
			names_buffer[--n] ='\0';
		}
		char* name_mem_aloc = (char*)malloc(sizeof(char)*(n+1));
		//printf("VALIDATING NAME WITHOUT WS: %ld",strlen(names_buffer));
		strcpy(name_mem_aloc,names_buffer);
		//name_mem_aloc[strlen(names_buffer)]='\0';//TODO: remove if not fix
		students[index]->name = name_mem_aloc;
		//printf("\nstudents[%d]->name = %s,student_len = %ld",i,students[i]->name,strlen(students[i]->name));
		index++;
	}
	if(pclose(pipe2)==-1){
		perror("pclose");
		exit(EXIT_FAILURE);
	}

	/*PART 3: constructing the full path to the src files and to the executables.*/

	for(int j=0;j<num_of_students;j++){
		//printf("students[j=%d] name is %s\n",j,students[j]->name);
		int path_to_subfolders_len = strlen(lines[0]);
		//printf("path_len = %d",path_to_subfolders_len);
		int subfolder_name_len = strlen(students[j]->name);
		//printf("subfolder_namne_len = %ld",subfolder_name_len);
		char* suffix = "/main.c";//TODO: CRITICAL: MAKE SURE THIS IS WHAT IS IN THE SUBFOLDERS ACCORDING TO THE REQUIREMENTS, NOT YOUR OWN GUESS.
		int suffix_len = strlen(suffix);
		int total_len = path_to_subfolders_len+subfolder_name_len+suffix_len+1;
		char* fullpath = (char*)malloc(total_len);
		strcpy(fullpath,lines[0]);
		strcpy(fullpath + path_to_subfolders_len,students[j]->name);
		strcpy(fullpath + path_to_subfolders_len+subfolder_name_len,suffix);
		students[j]->src_path = fullpath;

		char* exe_suffix = "/main.exe";
		int exe_suffix_len = strlen(exe_suffix);
		int total_len_exe = path_to_subfolders_len+subfolder_name_len+exe_suffix_len;
		char* exe_fullpath = (char*)malloc(total_len_exe);
		strcpy(exe_fullpath,lines[0]);
		strcpy(exe_fullpath + path_to_subfolders_len,students[j]->name);
		strcpy(exe_fullpath + path_to_subfolders_len + subfolder_name_len,exe_suffix);
		students[j]->exe_path = exe_fullpath;

		char* out_suffix = "/output";
		int out_suffix_len = strlen(out_suffix);
		int total_len_out = path_to_subfolders_len+subfolder_name_len+out_suffix_len;
		char* out_fullpath = (char*)malloc(total_len_out);
		strcpy(out_fullpath,lines[0]);
		strcpy(out_fullpath + path_to_subfolders_len,students[j]->name);
		strcpy(out_fullpath + path_to_subfolders_len + subfolder_name_len,out_suffix);
		students[j]->out_path = out_fullpath;
	}
	/*PART 4: compiling the students codes...*/
	for(int i=0;i<num_of_students;i++){
		//printf("for student %s, the path to the src is:\n %s\n",students[i]->name,students[i]->src_path);
		//printf("for student %s, the path to the exe is:\n %s\n",students[i]->name,students[i]->exe_path);
		//printf("for student %s, the path to the out is:\n %s\n",students[i]->name,students[i]->out_path);
		int status;
		pid_t pid = fork();
		if(pid==0){
			char* src_path = students[i]->src_path;
			char* exe_path = students[i]->exe_path;
			char* cmd[] = {"gcc",src_path,"-o",exe_path,NULL};
			execvp(cmd[0],cmd);
		}
		else{
			wait(&status);
			//printf("\nchild process finished.\n");
		}
	}
	/*PART 5: Extracting the inputs*/
	/*First, extract number of arguments by number of lines. TODO: if there is time,
	export all duplicate code segments, such as this, to functions.UPDATE-TODO:ran out of time...*/
	char bash_command[256];
	memset(bash_command,0,256);
	sprintf(bash_command, "wc -l %s",lines[1]);//I have chosen to use sprintf with command as a buffer, instead of doing another malloc and strcpy

	FILE *bash_pipe = popen(bash_command, "r");

	char bash_buffer[128];//I assume an input string will be less than 128. If not, I am in big problem.
	//read the output of wc -l
	if(fgets(bash_buffer,sizeof(bash_buffer),bash_pipe)==NULL){
		printf("failed to get amount of inputs");
		pclose(bash_pipe);
		exit(EXIT_FAILURE);
	}
	//printf("bash_buffer = %s",bash_buffer);
	int num_of_inputs = atoi(bash_buffer);
	//printf("\nnum_of_inputs = %d\n",num_of_inputs);
	if(pclose(bash_pipe)==-1){
		perror("pclose");
		exit(EXIT_FAILURE);
	}
	/*Done counting inputs*/

	FILE *input_file = fopen(lines[1], "r");//open the input file again, and now allocate space for each line
	char input_buffer[128];
	char* inputs[num_of_inputs+2];
	inputs[0]=NULL;
	inputs[num_of_inputs+1]=NULL;
	int input_counter = 0;
	while (fgets(input_buffer, sizeof(input_buffer), input_file) != NULL) {
		//printf("\nINPUT BUFFER = %s\n",input_buffer);
		int n = strlen(input_buffer);
		/*Removing whitespaces...*/
		while(n>0 && (input_buffer[n-1]=='\n' || input_buffer[n-1] == '\r' || input_buffer[n-1]==' '|| input_buffer[n-1]=='\t')){
			input_buffer[--n] ='\0';
		}
		//printf("	n=%d     ",n);
		char* allocated_space = (char*)malloc(n+1);
		strcpy(allocated_space,input_buffer);
		//allocated_space[n] = '\0';
		inputs[input_counter] = allocated_space;
		//printf("\ninputs[input_counter] = %s\n",inputs[input_counter]);
		//printf("\nlines[%d]=\n%s",counter,lines[counter]);
		input_counter++;
	}
	fclose(input_file);
	/*Now, inputs should contain all arguments needed for executing the Students main exe files.*/
	/*PART 6: running the users main.exe files and saving the results to a temporary file...*/
	for(int i = 0; i<num_of_students;i++){
		int status;
		pid_t pid = fork();
		if(pid==0){
			char* exe_path = students[i]->exe_path;
			char* out_path = students[i]->out_path;
			inputs[0] = (char*)malloc(strlen(exe_path)+1);
			strcpy(inputs[0],exe_path);
			freopen(out_path,"w",stdout);
			execvp(inputs[0],inputs);
		}
		else{
			wait(&status);
		}
	}

	/*FINAL PART: freeing dynamicaLLY allocated mem.*/
	for(int k =0 ;k<num_of_students;k++){
		free(students[k]->name);
		free(students[k]->src_path);
		free(students[k]->exe_path);
		free(students[k]->out_path);
		free(students[k]);
	}
	free(students);
	for(int j=0;j<3;j++){
		free(lines[j]);
	}
	for(int j =0;j<num_of_inputs;j++){
		free(inputs[j]);
	}

	return 0;
}
