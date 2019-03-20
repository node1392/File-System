#ifndef FILE_SYS_H
#define FILE_SYS_H

#include "dtree.h"      
#include "ldisk.h"

/* declare the file system datastructure */
typedef struct _FileSys { 
	TreeNode* root;  /* root of the tree */
	TreeNode* current;  /* current node */
	
	int disksz;
	int blocksz;
	
	int numblk;   /* total blocks */
	int numfree;  /* number of free blocks */
	
	LdiskNode* ldisk;

} FileSys;

/* initialize the file system */
void init_fs(FileSys* sys, int disksz, int blocksz);

/* print the directory */
void print_dir(TreeNode* node);

/* start the file system */
void run(FileSys* sys);


/* destroy the file system */
void destroy_fs(FileSys* sys);

/* create a new directory in current directory, return 0=okay
   return -1=duplicate name, -2=invalid dirname */
int mkdir(FileSys* sys, char* dname);

/* set specified directory as the current directory, if dname is ".." 
  set parent directory as current directory, if dname is "/"
  set root directory as current directory
  return 0=okay, -1=failed */
int cd(FileSys* sys, char* dname);

/* Print out the directory structure in breadth-first order */
void dir(FileSys* sys);

/* create the file in the current directoy, fsize is the number of bytes 
 return 0=okay, -1=duplicate name, -2=invalid name or size, -3=no spaces   
  -4=invalid current directory
*/
int create_file(FileSys* sys, char* fname, int fsize);

/* append the number of bytes(fsize) for the file
 return 0=okay, -1=invalid name or size, -2=no spaces
 */
int append_file(FileSys* sys, char* fname, int fsize);

/* remove the number of bytes for the file
 return 0=okay, -1=invalid name or size */
int remove_file(FileSys* sys, char* fname, int fsize);			

/* delete the node from the file system
 return 0=okay, -1=invalid name, -2=directoy not empty */
int delete_node(FileSys* sys, char* name);

/* get total size of the files */
int total_fsize(TreeNode* node);

/* display the file info */
void prfiles(TreeNode* node);

/* display the disk info */
void prdisk(FileSys* sys);

#endif

