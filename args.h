// File: args.h
// Purpose: parsing these three arguments for this program 
// Author: Brett Holman
// Note: after writing this, I discovered that C has Getopt.  I had previously used such
// argument parsing functions in Perl, and Python, but I didn't think that C had such things


int get_args(int argc, char *argv[], char *directory);
int iterate_args(int argc, char *argv[], char *directory);
int get_arg(char * arg, int init);


int iterate_args(int argc, char *argv[], char *directory){

	// Base case	
	if(argc==1) return 0;
	
	// Don't get args if it is the directory 
	if(argv[argc-1][0] != '-'){
        if(directory[0] != '\0'){
            return -1;
        }
		strcpy(directory, argv[argc-1]); 	
		
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
	}else if(arg[init]=='a'){
		args |= ARG_a;
	}else if(arg[init]=='R'){
		args |= ARG_R;
	}else{
        return -1;
    }

	// Check the other characters
	return args | get_arg(arg, ++init);
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
    //printf("args found:%d", args);
    if(!directory[0])
    	directory[0]='.';

    return args;
} 
