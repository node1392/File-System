/***************************************************
 *  Edrick Ramos			           *
 *  File System Management         		   *
 ***************************************************/


/*
 The main file, parses the arguments and initializes the file system.

 */

#include <stdio.h>
#include <stdlib.h>   
#include <string.h>

#include "filesys.h"

/* initialize the file system from the argument, return 0 if failed */
FileSys* init_build(int argc, char* argv[]);

/* initialize the dir file */
void init_dir(FileSys* sys, char* dirfile);

/* initialize the info file */
void init_file(FileSys* sys, char* infofile);

int main(int argc, char* argv[]) {
	FileSys* sys = init_build(argc, argv);
	if (sys == 0) {
		printf("Usage: %s -f [%s] -d [%s] -s [%s] -b [%s]\n"
		  , argv[0], "infofiles", "dirfiles", "disksize", "blocksize");
	}
	else {
		run(sys);         /* run the file system */
		destroy_fs(sys);  /* destroy the file system */
	}
	    
	return 0;
}
                             
/* initialize the file system from the argument, return 0 if failed */
FileSys* init_build(int argc, char* argv[]) {
	int blocksz = -1;   /* block size */
	int disksz = -1;  /* disk size */
	int i;
	FileSys* sys = NULL;
	FILE* file;
	
	/* get disk size */
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0 && i+1 < argc) {
			disksz = atoi(argv[i+1]);
			break;
		}
	}
	          
	/* get disk size */
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-b") == 0 && i+1 < argc) {
			blocksz = atoi(argv[i+1]);
			break;
		}
	}
	
	if (disksz <= 0 || blocksz <= 0 || disksz/blocksz <= 0)
		return 0;
	
	/* create the file system */	
	sys = (FileSys*)malloc(sizeof(FileSys));
	init_fs(sys, disksz, blocksz);
	
	/* init dirs  */
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-d") == 0) {
			for(i++; i < argc && argv[i][0] != '-'; i++) {
				init_dir(sys, argv[i]);
			}		
			break;
		}
	}      
	/* init files  */  
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0) {
			for(i++; i < argc && argv[i][0] != '-'; i++) {
				init_file(sys, argv[i]);
			}		
			break;
		}
	}      
	
	return sys;
}
	
/* initialize the dir file */
void init_dir(FileSys* sys, char* dirfile) {
	FILE* fp = fopen(dirfile, "r");
	char buf[1024];
	char* tokens[128];
	int ntok = 0, i;
	char* dname;
	
	while (fscanf(fp, "%s", buf) == 1) {   
		/* buf is the full path of the directoy, for example:
		   a/b/c/d, the program will first create a, then b, then c... */ 
		dname = strtok(buf, "/\\");
		ntok = 0;   
		while (dname != 0) {
			tokens[ntok++] = dname;
			dname = strtok(NULL, "/\\");
		}
		
		sys->current = NULL;
		for (i = 0; i < ntok; i++) {
			/* create the directoy and enter it */
			mkdir(sys, tokens[i]);
			cd(sys, tokens[i]);			
		}	
	}
	
	fclose(fp);
}

/* initialize the info file */
void init_file(FileSys* sys, char* infofile) {   
	FILE* fp = fopen(infofile, "r"); 
	char buf[1024];    
	char* tokens[128];
	int ntok = 0, i;
	char* dname;
	int fsize = 0;
	int ch;   
	
	while (fscanf(fp, "%s", buf) == 1) {
		fscanf(fp, "%s", buf);  /* skip blocks */    
		fscanf(fp, "%s", buf);  /* skip file mode */    
		fscanf(fp, "%s", buf);  /* skip link */     
		fscanf(fp, "%s", buf);  /* skip owner */   
		fscanf(fp, "%s", buf);  /* skip group */
		fscanf(fp, "%d", &fsize);   /* read file size */
		
		/* skip date */
		ch = fgetc(fp);
		while (ch != ':')
			ch = fgetc(fp);  
		fscanf(fp, "%s", buf);  /* skip time */   
		fscanf(fp, "%s", buf);  /* get the full path */ 		
	               
		/* buf is the full path of the file, for example:
		   a/b/c/d.txt, the program will first create a, then b, then c...
		   finally create the file */
		dname = strtok(buf, "/\\");                                          
		ntok = 0;   
		while (dname != 0) {
			tokens[ntok++] = dname;
			dname = strtok(NULL, "/\\");
		}             
		
		sys->current = NULL;
		for (i = 0; i < ntok - 1; i++) {   
			mkdir(sys, tokens[i]);
			cd(sys, tokens[i]);
		} 		
		
		if (ntok > 0) {
			create_file(sys, tokens[ntok-1], fsize);
		}	
	}
	
	fclose(fp);
}

