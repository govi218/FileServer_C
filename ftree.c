#include <stdio.h>
#include "ftree.h"
#include "hash.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <libgen.h>


/*void copy_file(struct fileinfo* filestruct) {
	char a;
	strcat(dest,"/");
	strcat(dest,basename(src));
	//open read and write streams
	FILE* read;
	FILE* write;

	read = fopen(src, "r");
	//chdir(dest);
	write = fopen(dest, "w");

	//error checking
	if (read == NULL) //|| (write == NULL))
	{	
		perror("Read Error: ");
		exit(0);
	}

	else if (write == NULL)
	{
		perror("Write Error: ");
		exit(0);
	}

	//write from src to dest char by char
	while (1){
    	a = fgetc(read);
    	if (a == EOF) 
    	{
    		break;
    	}
    	fputc(a, write);
   	}
 		
    struct stat src_st;

    if(fstat(fileno(read), &src_st)){
		perror("stat: ");
		exit(EXIT_FAILURE);
	}

	if(chmod(basename(dest), src_st.st_mode & 07777)){
		perror("chmod: ");
		exit(EXIT_FAILURE);
	}
  	//close files
   	fclose(read);
   	fclose(write);
	dest = dirname(dest);

}*/

void hash_copy(struct fileinfo* filestruct, int fd) 
{
	printf("dfg\n");
	//declarations/initialization
	char* dest = strdup(filestruct->path); //get dir to check
	char* dest_dirname = dirname(dest);			
	/*FILE* dest_hash = fopen(filestruct->path, "r");		//open file in server*/
	DIR* dest_dir;
	dest_dir = opendir(dest_dirname);					//open dir to check if file exists
	struct dirent* dr;
	//Error check	
	if (dest_dir == NULL)
	{
		perror("File Path Error: ");
		exit(EXIT_FAILURE);
	}

	//Loop through files in dest directory
	while ((dr = readdir(dest_dir)) != NULL) 
	{

		//if conflicting file names, write mismatch
    if (strcmp(basename(filestruct->path), dr->d_name) == 0)
    {	
			FILE* dest_hash = fopen(dr->d_name, "r");
			struct stat st;
			stat(dr->d_name, &st);
			if (st.st_size == filestruct->size)
			{
				if (check_hash(hash(dest_hash), filestruct->hash) == 1)
				{
					int match = MATCH;
    			write(fd, &match, sizeof(int));
  				fclose(dest_hash);
					closedir(dest_dir);
					free(dest);
  				return;
				}
			}
		}
	}
			//printf("dfgb\n");
			int mismatch = MISMATCH;
    	write(fd, &mismatch, sizeof(int));

			FILE* writer = fopen(filestruct->path, "a");
			printf("%s\n", filestruct->path);
    	int bytes;
    	char buf[1];
/*    	while(bytes > 0)
    	{
    		printf("%s %d\n", buf, bytes);
				bytes = read(fd, buf, sizeof(buf));
				write(fileno(writer), buf, sizeof(buf));
    	}*/
    	while((bytes = read(fd, buf, sizeof(buf))) > 0)
    	{
    		printf("%s %d\n", buf, bytes);
				write(fileno(writer), buf, sizeof(buf));
    	}
    //if same file name was not found, go ahead and copy
//	copy_file(src, dest);
  //close anything open
	closedir(dest_dir);
	free(dest);


}


int exists(char *dest_path, char *dir_name) {
	DIR *destination = opendir(dest_path);
	struct dirent *destent;
	while ((destent = readdir(destination)) != NULL) {
		if (strcmp((*destent).d_name,dir_name) == 0) {
			return 1;
		}
	}
	return 0;
}


void copy_ftree(struct fileinfo* filestruct, int fd) 
{
	//Is a Directory
	if(S_ISDIR(filestruct->mode)){
		char* path = strdup(filestruct->path);
		char* file_name = basename(path);
		char* dir_name = dirname(path);
		if (exists(dir_name,file_name)) {
			chmod(filestruct->path, filestruct->mode & 07777);
		} else {
			mkdir(filestruct->path, filestruct->mode & 07777);
		}
	  int match = MATCH;
	  write(fd, &match, sizeof(int));
	}
	//Is a Regular File
	else if(S_ISREG(filestruct->mode))
	{
		//printf("in isreg\n");
		hash_copy(filestruct, fd);
	}
}
/*	DIR *source = opendir(src_path); 

	// points to first directory entry INSIDE source
	struct dirent *srcent;

	while ((srcent = readdir(source)) != NULL) {
		// Skip files starting with '.'
		while ((srcent!= NULL) && (srcent->d_name[0] == '.')) {
			srcent = readdir(source); //reads next file
		}

		strcat(src_path,"/");
		strcat(src_path,srcent->d_name);
		if (stat(src_path, &src_buff)) {
			perror("stat2: ");
			break;
		}

		if(S_ISDIR(src_buff.st_mode) == 1){
			copy_ftree(src_path,dest_path);

		} else if(S_ISREG(src_buff.st_mode)) {
			hash_copy(src_path, dest_path);
			src_path = dirname(src_path);
		}
	}*/


			/*		  //get status for src
    	struct stat src_st;
    	struct stat dest_st;
    	char* dest_path = strdup(dest);
    	strcat(dest_path, "/");
    	strcat(dest_path, dr->d_name);
    	if (stat(src, &src_st) != 0) {
				perror("stat: ");
				exit(0);
			}

			if (stat(dest_path, &dest_st) != 0) {
				perror("stat: ");
				exit(0);
			}

        	//if the sizes match, compare hash
        	if ((src_st.st_size) == (dest_st.st_size))
        	{
            	//open conflicting file to prepare for hash

            	src_hash = fopen(src, "r");
	        	//chdir(dest);
				dest_hash = fopen(dest_path, "r");

				//error checking
				if ((dest_hash == NULL) || src_hash == NULL)
				{
					perror("Hash Path Error: ");
					exit(EXIT_FAILURE);
				}

	        	//if hashes are not the same, overwrite
   		     	if (strcmp(hash(src_hash), hash(dest_hash)) != 0)
	       	 	{
					//chdir(src_dir);
					free(dest_path);
	        		copy_file(src, dest);
	        	    fclose(dest_hash);
	        	    closedir(dest_dir);
	        	    fclose(src_hash);
	        	    return;
   		     	}

   		     	//if hashes are the same, skip
   	  			if(fstat(fileno(src_hash), &src_st)){
					perror("stat: ");
					exit(EXIT_FAILURE);
				}
   	   		    if(chmod(dr->d_name, src_st.st_mode & 07777)){
					perror("chmod: ");
					exit(EXIT_FAILURE);
				}	     	
   		     	free(dest_path);
        	    fclose(dest_hash);
   		     	return;	    		
        	}

     		//if they have different sizes, overwrite
     		free(dest_path);
    		copy_file(src, dest);      
        	return;*/ 