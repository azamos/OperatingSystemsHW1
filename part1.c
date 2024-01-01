#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
	if(argc<3){
		printf("Please provide 2 file paths. exiting...");
		return 0;
	}

	FILE *file1 = fopen(argv[1],"r");
	if (file1 == NULL){
		printf("ERROR reading file");
		return -1;
	}
	char *line1 = NULL;
	size_t len1 = 0;
	ssize_t read1;
	
	FILE *file2 = fopen(argv[2],"r");
	if (file2 == NULL){
		printf("ERROR reading file");
		return -1;
	}
	char *line2 = NULL;
	size_t len2 = 0;
	ssize_t read2;
	while(1){
		read1 = getline(&line1,&len1,file1);
		read2 = getline(&line2,&len1,file2);
		if(read1==-1 && read2 == -1){
			fclose(file1);
			fclose(file2);
			printf("\nFILES are the same\n");
			return 2;
		}
		if((read1==-1)^(read2==-1)){
			fclose(file1);
			fclose(file2);
			printf("\nreturned FALSE, since one file finished reading before the other, implying different files\n");
			return 1;
		}
		if(strcmp(line1,line2)!=0){
			fclose(file1);
			fclose(file2);
			printf("FALSE. have found a mismatching line\nline1=%s\nline2=%s\n",line1,line2);
			return 1;/*not equal, due to different line*/
		}
	}
}
