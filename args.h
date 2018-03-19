// File: args.h
// Purpose: parsing these three arguments for this program 
// Author: Brett Holman
// Note: after writing this, I discovered that C has Getopt.  I had previously used such
// argument parsing functions in Perl, and Python, but I didn't think that C had such things


int get_args(int argc, char *argv[], char *directory);
int validate_args(int argc, char *argv[]);
int iterate_args(int argc, char *argv[], char *directory);
int get_arg(char * arg, int init);

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
			//	printf("flag -l found\n");
			}else if(character=='a'){
				args |= ARG_a;
			//	printf("flag -a found\n");
			}else if(character=='R'){
				args |= ARG_R;
			//	printf("flag -R found\n");
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
		//printf("flag -l found\n");
	}else if(arg[init]=='a'){
		args |= ARG_a;
		//printf("flag -a found\n");
	}else if(arg[init]=='R'){
		args |= ARG_R;
		//printf("flag -R found\n");
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
