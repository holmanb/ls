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
#include "args.h"

int ls(char *directory, struct dirent **namelist, int args);
int recursive_print_files(int num, struct dirent **namelist, int args, char *directory, char *buffer);
int print_files(int num, struct dirent **namelist, int args, char *directory, char *buffer, char *names, int num_name);
int get_args(int argc, char *argv[], char *directory);
int validate_args(int argc, char *argv[]); // this is currently not implemented (TODO)
int iterate_args(int argc, char *argv[], char *directory);
int get_arg(char * arg, int init);
int is_hidden_file(char *name);
int print_norm(struct stat stats, char *filename);
int print_l(struct stat stats , char *filename);

int main(int argc, char *argv[])
{
   struct dirent **namelist;
   int n=-1;
   int args=0;
   char directory[LENGTH]={0};
   char str_buffer[LENGTH]={0};
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

	// possible a single file, try doing lstat on it
	// currently doesn't handle a list of files, consider implementing this (TODO)
	struct stat stat_struct;
	if(lstat(directory, &stat_struct)==-1){

		// This is not a filename or location, 
		printf("%s\n", strerror(errno));
		return -1;
	}else{
		// a single file was passed in
		printf("Directory listing of %s\n", directory);
		printf("Size\tName\n");
		printf("%lu\t%s\n ", stat_struct.st_size, directory);
	}
   }else{

	printf("Directory listing of %s\n", directory);
	printf("Size\tName\n");
	ls(directory,namelist, args);
   }
   return 0;
}

int ls(char *directory, struct dirent **namelist, int args){
	
	char str_buffer[LENGTH];
	int n=scandir(directory, &namelist, NULL, alphasort); 
	if(n<0){
		// possible a single file, try doing lstat on it
		// currently doesn't handle a list of files, consider implementing this (TODO)
		struct stat stat_struct;
		if(lstat(directory, &stat_struct)==-1){

			// This is not a filename or location, 
			printf("%s :%s\n", strerror(errno), directory);
			return -1;
		}else{
			// a single file was passed in
			printf("%lu\t%s\n ", stat_struct.st_size, directory);
		  return -1;
		}
	}
	char  directory_buffer[LENGTH]={0}, names[LENGTH*32]={0};
	int num_name=0;
	num_name = print_files(n, namelist, args, directory,str_buffer, names, num_name);

	// Getting the name of the directory
	if(args&ARG_R){	

		printf("\n%s\n", names);
		int i=0,j=0;
		while(num_name--)
		{
			j=0;
			while(names[i]!='\n'){
				directory_buffer[j++]=names[i++];
			}
			directory_buffer[++i]='\0';
			ls(directory_buffer, namelist, args);
		}
	}
	return 0;
}

// prints out each file
int print_files(int num, struct dirent **namelist, int args, char *directory, char *buffer, char *names, int num_name){

	// Base case
	if(!num) return 0;	 	

	// Recur	
	num_name += print_files(--num, namelist, args, directory, buffer, names, num_name);	

	// concatenate filename and filepath
	strcpy(buffer, directory);		
	strcat(buffer, "/");
	strcat(buffer, namelist[num]->d_name);
	strcat(buffer, "\0");

	// print file if -a or if it isn't hidden
	if(args & ARG_a || !is_hidden_file(namelist[num]->d_name)){

		// Getting stat struct
		struct stat stats;
		if(lstat(buffer, &stats)==-1){
			printf("%s\n", strerror(errno));
		}

		// Recursive flag set
		if(args&ARG_R){

			// Check if it's a directory
			if(S_ISDIR(stats.st_mode)){
				
				// skip '.' and '..' directories
				int dot_dot = (!strcmp(".", namelist[num]->d_name) || !strcmp("..", namelist[num]->d_name));
				
				if(!dot_dot){
					strcat(names, buffer);
					strcat(names, "\n");	
					num_name++;
				}
			}
		}

		// different print function for -l flag
		if(args & ARG_l){
			print_l(stats, namelist[num]->d_name);
		}
		else{
			print_norm(stats, namelist[num]->d_name);
		}
	}
	free(namelist[num]);
	return num_name;
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

/* "normal print", used when -l not specified*/
int print_norm(struct stat stats, char *filename){

	
	printf("%ld\t%s\n", stats.st_size, filename); 
	return 0;
}

/* "print_l", used when -l specified*/
int print_l(struct stat stats , char *filename){
	printf("%ld\t%s\n", stats.st_size, filename); 
	return 0;
}
