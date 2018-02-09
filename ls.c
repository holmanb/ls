/* print files in current directory in reverse order */

#define _SVID_SOURCE
#define ARG_l 1
#define ARG_a 2
#define ARG_R 4
#define LENGTH 1024

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_files(int num, struct dirent **namelist);
int get_args(int argc, char *argv[], char *directory);
int validate_args(int argc, char *argv[]);

int main(int argc, char *argv[])
{
   struct dirent **namelist;
   int n, args;
   char directory[LENGTH];

   // Validate arguments
   if(validate_args(argc, argv)){
	printf("invalid arguments\n");
	return -1;
   }
   
   // stores args and gets the directory if there is one 
   args = get_args(argc, argv, directory); 

   // do scandir and print out the directory
   n = scandir(directory, &namelist, NULL, alphasort);
   if (n < 0){
       perror("scandir");
   }else {
	printf("Directory listing of %s\n", directory);
	print_files(n, namelist);
   }
}


// prints out each file
void print_files(int num, struct dirent **namelist){

	// Base case
	if(!num) return;	 	

	print_files(--num, namelist);	
	printf("%s\n", namelist[num]->d_name);
	
	free(namelist[num]);
}

// interpret CLI args
int get_args(int argc, char *argv[], char *directory){

   int args=0;
   
   // if length == 1, no args passed 
   if(argc == 1){
	directory[0] = '.';
	return 0;
   }
    
   // For this simple program, the only non-flag is the distination to ls
   int directory_found=0;
   for (int i=1;i<argc;i++){

	// Searching for non-flag arg
	if(argv[i][0] != '-'){
		strcpy(directory, argv[i]); 	
		directory_found=1;
	}
	// checking flags
	else{
	   	int j=1;
		char character;
		while(character=argv[i][j++]){
			if(character=='l'){
				args |= ARG_l;
				printf("flag -l found\n");
			}else if(character=='a'){
				args |= ARG_a;
				printf("flag -a found\n");
			}else if(character=='R'){
				args |= ARG_R;
				printf("flag -R found\n");
			}	
		}
	}
   }
   if(!directory_found)
	directory[0]='.';

   return args;
} 

// validate CLI args
int validate_args(int argc, char *argv[]){
	return 0;
	// validate flags against list (alR)
	// validate only one non-flag argument
}
