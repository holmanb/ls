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

void print_files(int num, struct dirent **namelist, int args, char *directory, char *buffer);
int get_args(int argc, char *argv[], char *directory);
int validate_args(int argc, char *argv[]); // this is currently not implemented (TODO)
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

	// possible a single file, try doing lstat on it
	// currently doesn't handle a list of files, consider implementing this (TODO)
	struct stat stats;
	if(lstat(directory, &stats)==-1){

		// This is not a filename or location, 
		printf("%s\n", strerror(errno));
		perror("scandir");
		return -1;
	}else{
		printf("%u\t%s\n ", stats.st_size, directory);
	}
   }else{
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
		if(lstat(directory, &stats)==-1){
		//if(!lstat(buffer, &stats)){
			printf("%s\n", strerror(errno));
		}
		else{
			printf("%u\t%s\n", stats.st_size, namelist[num]->d_name);
		}
	}
	// only print unhidden files 
	else if(!hidden){
		if(lstat(directory, &stats)==-1){
		//if(!lstat(buffer, &stats)==1){
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


