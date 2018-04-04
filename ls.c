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
#define _DEFAULT_SOURCE 1
#define LENGTH 1024

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <errno.h>
#include "args.h"

int print_dir(int args, char * directory);
int recur(int args, char * directory);
int ls(char *directory, struct dirent **namelist, int args);
int recursive_print_files(int num, struct dirent **namelist, int args, char *directory, char *buffer);
int print_files(int num, struct dirent **namelist, int args, char *directory, char *buffer, char *names, int num_name);
int get_args(int argc, char *argv[], char *directory);
int validate_args(int argc, char *argv[]); // this is currently not implemented (TODO)
int iterate_args(int argc, char *argv[], char *directory);
int get_arg(char * arg, int init);
int is_hidden_file(char *name);
int print_norm(struct stat stats, char *filename);
int print_l(struct stat stats , char *filename, char *path);

int main(int argc, char *argv[])
{
    int args=0;
    char directory[LENGTH]={0};
    char str_buffer[LENGTH]={0};

    // PROF requested ls to default to -a behavior 
    // comment out next line for standard ls behavior
    args=DEFAULT_ARGS;

    // Validate arguments
    if(validate_args(argc, argv)){
        printf("invalid arguments\n");
        return 2;
    }

    // stores args and gets the directory if there is one 
    args = get_args(argc, argv, directory) | args; 

    print_dir(args, directory);
    printf("directory %s\n", directory);
    // do recursion after printing directory
    if(args & ARG_R){

        printf("directory %s\n", directory);
        // skip '.' and '..' directories
        if(!(strcmp(".", directory) && strcmp("..",directory)) || !strcmp("/", directory)){ 
            recur(args, directory); 
        }
        else{printf("looks like [%s] is equal to [.] or [..] \n",directory);}
    }
    return 0;
}

int print_dir(int args, char * directory){

    // do scandir and print out the directory
    int n = -1;
    struct dirent **namelist;
    struct stat stat_struct;
    n=scandir(directory, &namelist, NULL, alphasort); 
    if (n < 0){

        // possible a single file, try doing lstat on it
        // currently doesn't handle a list of files, consider implementing this (TODO)
        if(lstat(directory, &stat_struct)==-1){

            // This is not a filename or location, 
            printf("Error in print_dir on file: [%s]\n", directory);
            printf("%s\n", strerror(errno));
            return 1;
        }else{
            // a single file was passed in
            printf("Size\tName\n");
            printf("%lu\t%s\n ", stat_struct.st_size, directory);
        }
    }else{

        // This is where the listing of directories begins
        printf("Directory listing of %s\n", directory);
        int num=-1;
        while(++num < n){

            // allows hidden files and directoriesto be skipped
            if(args & ARG_a || !is_hidden_file(namelist[num]->d_name)){

                // Check the file
                int str_size = strlen(directory) + strlen(namelist[num]->d_name) + 3;
                char file[LENGTH*4];
                strcpy(file, directory);
                strcat(file, "/");
                strcat(file, namelist[num]->d_name);
                strcat(file, "\0");

                if(lstat(file, &stat_struct)==-1){
                    printf("Error in print_dir on file: [%s]\n", file);
                    printf("%s\n", strerror(errno));
                    return 1;
                }

                // different print function for -l flag
                if(args & ARG_l){
                    print_l(stat_struct, namelist[num]->d_name, directory);
                }

                // default print 
                else{
                    print_norm(stat_struct, namelist[num]->d_name);
                }
            }
            free(namelist[num]);
        }
        free(namelist);
    }
    return 0;
}
int recur(int args, char * directory){

    int len = sizeof(directory) / sizeof(directory[0]);

    int n = -1;
    int num=-1;
    struct dirent **namelist;
    struct stat stat_struct;
    n=scandir(directory, &namelist, NULL, alphasort); 
    if (n < 0){

        // possible a single file, try doing lstat on it
        // currently doesn't handle a list of files, consider implementing this (TODO)
        if(lstat(directory, &stat_struct)==-1){

            // This is not a filename or location, 
            printf("Error in print_dir on file: [%s]\n", directory);
            printf("%s\n", strerror(errno));
            return 1;
        }else{
            // a single file was passed in
            printf("Size\tName\n");
            printf("%lu\t%s\n ", stat_struct.st_size, directory);
            return 0; 
        }
    }

    // Scan the directory
    printf("\n%s:\ntotal %d\n", directory, n);

    // Check each item to see if it is a directory 
    while(++num < n){
        if(!(strcmp(".", namelist[num]->d_name) && strcmp("..",namelist[num]->d_name))){ 
            free(namelist[num]);
            continue;
        }
        int str_size = strlen(directory) + strlen(namelist[num]->d_name) + 3;
        char file[LENGTH*4];
        strcpy(file, directory);
        strcat(file, "/");
        strcat(file, namelist[num]->d_name);
        strcat(file, "\0");

        // Check the file
        if(lstat(file, &stat_struct)==-1){
            printf("Error in print_dir on file: [%s]\n", file);
            printf("%s\n", strerror(errno));
            free(namelist[num]);
            return 1;
        }
        
        // if it's a directory
        if(S_ISDIR(stat_struct.st_mode)){
            print_dir(args, file);
            recur(args, file);
        }

        free(namelist[num]);
    }
    free(namelist);
    return 0;
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
int print_l(struct stat stats , char *filename, char * path){

    // Mode
    mode_t mode =  stats.st_mode;
    char perm[64]={0};
    char slink[64] = {'\0'};

    // File Type
    switch(mode & S_IFMT){
        case S_IFCHR:
            strcat(perm, "c");
            break;
        case S_IFBLK:
            strcat(perm, "b");
            break;
        case S_IFIFO:
            strcat(perm, "p");
            break;
        case S_IFDIR:
            strcat(perm, "d");
            break;
        case S_IFLNK:
            strcat(perm, "l");
            break;
        case S_IFSOCK:
            strcat(perm, "s");
            break;
        case S_IFREG:
            strcat(perm, "-");
            break;
        default:
            strcat(perm, "?");
            break;
    }

    // Permissions
    mode_t  m[9] = {
        S_IRUSR, S_IWUSR, S_IXUSR, 
        S_IRGRP, S_IWGRP, S_IXGRP,
        S_IROTH, S_IWOTH, S_IXOTH
        };
    for(int i=0; i<9; i++){
        switch(m[i] & mode){
            case S_IRUSR:
            case S_IRGRP:
            case S_IROTH:
                strcat(perm, "r");
                break;
            case S_IWUSR:
            case S_IWGRP:
            case S_IWOTH:
                strcat(perm, "w");
                break;
            case S_IXUSR:
            case S_IXGRP:
            case S_IXOTH:
                strcat(perm, "x");
                break;
            default:
                strcat(perm, "-");
                break;
        }
    }

    // SUID
    if(mode & S_ISUID)
        perm[3] = (mode & S_IXUSR) ? 's' : 'S';
    // GUID
    if(mode & S_ISGID)
        perm[6] = (mode & S_IXGRP) ? 's' : 'i';
    // Sticky bit
    if(mode & S_ISVTX)
        perm[9] = (mode & S_IXOTH) ? 't' : 'T';

    // User
    struct passwd *usr = getpwuid(stats.st_uid);
     
    // Group
    struct group * grp = getgrgid(stats.st_gid);

    
    // Last Modified Time
    const double YEAR = 365*24*3600;
    struct tm *time_struct;
    time_t now, modified = stats.st_mtime;
    char buf[80];
    time(&now);
    time_struct = localtime(&modified);
    double time_diff = difftime(now, modified); 
    char format[15] = " %b %d ";

    // Display files that are over a year old using year rather than time
    if(time_diff > YEAR)
        strcat(format,  " %Y");
    else
        strcat(format,  "%R");

    strftime(buf, sizeof(buf), format, time_struct);

    //symbolic link
    if( perm[0] == 'l' )
        readlink(path, slink, 64);
    char arrow[5]={' '};;
    
    if(slink[0] != '\0')
        strcat(arrow, " -> ");

    

	printf("%s %3d %s %s %7ld %s %s %s %s\n", perm, stats.st_nlink, usr->pw_name, grp->gr_name, stats.st_size, buf, filename, arrow, slink); 
	return 0;
}
