/* ls command, accepting -l -a and -R arguments
 *
 * Author: Brett Holman
 * Spring 2018, UW Stout
 */


#define _SVID_SOURCE
#define ARG_l 1
#define ARG_a 2
#define ARG_R 4
#define DEFAULT_ARGS ARG_a
#define LENGTH 1024

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

void print_files(int num, struct dirent **namelist, int args, char *directory, char *buffer);
int get_args(int argc, char *argv[], char *directory);
int validate_args(int argc, char *argv[]);
int iterate_args(int argc, char *argv[], char *directory);
int get_arg(char * arg, int init);
int is_hidden_file(char *name);

int main(int argc, char *argv[])
{
   struct dirent **namelist;
   int n=-1;
   int args=0;
   char directory[LENGTH]={0};
   char str_buffer[LENGTH];
   directory[0] = '\0';

   // PROF requested ls to default to -a behavior 
   // comment out next line for standard ls behavior
   args=DEFAULT_ARGS;

   // Validate arguments
   if(validate_args(argc, argv)){
	printf("invalid arguments\n");
	return -1;
   }
   
   // stores args and gets the directory if there is one 
   args = get_args(argc, argv, directory) | args; 

   // do scandir and print out the directory
   n=scandir(directory, &namelist, NULL, alphasort); 
   if (n < 0){
	printf("directory: %s\n", directory);
       perror("scandir");
   }else {
	printf("Directory listing of %s\n", directory);
	printf("Size\tName\n");
	print_files(n, namelist, args, directory,str_buffer);
   }
   return 0;
}


// prints out each file
void print_files(int num, struct dirent **namelist, int args, char *directory, char *buffer){

	// Base case
	if(!num) return;	 	

	print_files(--num, namelist, args, directory, buffer);	

	// Skip hidden files unless -a
	int hidden = is_hidden_file(namelist[num]->d_name);		

	// concatenate filename and filepath
	strcpy(buffer, directory);		
	strcat(buffer, "/");
	strcat(buffer, namelist[num]->d_name);
	struct stat stats;

	// arg -a
	int print_all = args&2;
	if(print_all){
		if(lstat(buffer, &stats)==1){
			printf("%s\n", strerror(errno));
		}
		else{
			printf("%u\t%s\n", stats.st_size, namelist[num]->d_name);
		}
	}
	// only print unhidden files 
	else if(!hidden){
		if(lstat(buffer, &stats)==1){
			printf("%s\n", strerror(errno));
		}
		else{
			printf("%u\t%s\n", stats.st_size, namelist[num]->d_name);
		}
	}
		//printf("%s\n", namelist[num]->d_name);
	
	free(namelist[num]);
}

int is_hidden_file(char *name){

	// hidden files start with '.'
	if(name[0] != '.') return 0;
	if(name[1] == '\0') return 0;	       // . is not a hidden file
	if(name[1] == '.' && name[2] == '\0')  //.. is not a hidden file
		return 0;	

	// all other files have regex pattern ^\..+$ and are therefore hidden
	return 1;
}


// interpret CLI args
int get_args_iter(int argc, char *argv[], char *directory){

   int args=0;
   
   // if length == 1, no args passed 
   if(argc == 1){
	directory[0] = '.';
	directory[1] = '\n';
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
   if(!directory_found){
	directory[0]='.';
	directory[1]='\0';
   }
   return args;
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
   args = iterate_args(argc, argv, directory);
   printf("args found:%d", args);
   if(!directory[0])
	directory[0]='.';

   return args;
} 

// validate CLI args
int validate_args(int argc, char *argv[]){
	return 0;
	// validate flags against list (alR)
	// validate only one non-flag argument
}



int iterate_args(int argc, char *argv[], char *directory){

	// Base case	
	if(argc==1) return 0;
	
	// Don't get args if it is the directory 
	//printf("argc:%d \n ", argc); 
	//printf("argv:%s\n",  argv[argc-1]);
	if(argv[argc-1][0] != '-'){
		//printf("argv[argc-1][0]=%d\n", argv[argc-1][0]);
		strcpy(directory, argv[argc-1]); 	
		//printf("directory found in arglist: %s\n", directory);	
		
		// don't get the character for the directory
		return iterate_args(--argc, argv, directory);
	}
	else{
		return iterate_args(--argc, argv, directory) | get_arg(argv[argc], 1);	
	}
}	

int get_arg(char * arg, int init){

	// Searching for non-flag arg
	// checking flags
	int args=0;

	// End of char* (base case)
	if(!arg[init]) return 0;

	// Check for args
	if(arg[init]=='l'){
		args |= ARG_l;
		printf("flag -l found\n");
	}else if(arg[init]=='a'){
		args |= ARG_a;
		printf("flag -a found\n");
	}else if(arg[init]=='R'){
		args |= ARG_R;
		printf("flag -R found\n");
	}	

	// Check the other characters
	return args | get_arg(arg, ++init);
}
