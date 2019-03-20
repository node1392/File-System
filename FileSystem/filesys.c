/*
 Implement the file system
 */

#include <stdio.h>
#include <stdlib.h>   
#include <string.h>

#include "filesys.h"


/* initialize the file system */
void init_fs(FileSys* sys, int disksz, int blocksz) {
	sys->current = NULL;
	sys->root = NULL;
	sys->disksz = disksz;
	sys->blocksz = blocksz;  
	sys->numblk = disksz / blocksz;
	sys->numfree = sys->numblk;
	
	sys->ldisk = create_disknode(0, sys->numblk - 1, 0);	
} 
  
/* print the directory */
void print_dir(TreeNode* node) {
	if (node == NULL) {
		printf("/");
	}
	else {
		print_dir(node->parent);
		printf("%s/", node->name);
	}
}

/* start the file system */
void run(FileSys* sys) {
	char buf[1024];
	int result, i;
	
	sys->current = sys->root;
	while (1) { 
		fflush(stdin);
		printf("\n");
		print_dir(sys->current);
		printf(" $> ");  /* print the start symbol */
		fflush(stdout);
		scanf("%s", buf);  /* read the command */
		
		if (strcmp(buf, "exit") == 0) {
			break;
		}
		else if (strcmp(buf, "mkdir") == 0) {   
			scanf("%s", buf);  /* read the name */
			result = mkdir(sys, buf);
			if (result == 0) {
				printf("dir %s created.\n", buf);
				if (sys->current == NULL)
					sys->current = sys->root;
			}
			else if (result == -1)
				printf("Error: %s already exists\n", buf);
			else 
				printf("Error: invalid dir name %s\n", buf);
		}                  
		else if (strcmp(buf, "cd") == 0) {  
			scanf("%s", buf);  /* read the path */
			result = cd(sys, buf);      
			if (result == -1)
				printf("Error: %s does not exist\n", buf);
		}                           
		else if (strcmp(buf, "ls") == 0) {  
			if (sys->current != NULL) {
				for (i = 0; i < sys->current->childsize; i++) {
					if (sys->current->children[i]->type == 0)
						printf("[");
					printf("%s", sys->current->children[i]->name); 
					if (sys->current->children[i]->type == 0)
						printf("]");
					printf("\t");
				}
			}
			printf("\n");
		}
		else if (strcmp(buf, "dir") == 0) {
			dir(sys);
		}
		else if (strcmp(buf, "create") == 0) {  
			scanf("%s", buf);  /* read the name */
			result = create_file(sys, buf, 0);
			if (result == 0) 
				printf("file %s created\n", buf);
			else if (result == -1)
				printf("Error: file %s already exist\n", buf);
			else if (result == -2)
				printf("Error: invalid name\n");
			else if (result == -3)
				printf("Out of space\n");
			else if (result == -4)
				printf("Error: invalid current directory\n");
		}
		else if (strcmp(buf, "append") == 0) {
			scanf("%s %d", buf, &i);
			result = append_file(sys, buf, i);  
			if (result == 0) 
				printf("file %s appended\n", buf);
			else if (result == -1)
				printf("Error: invalid file or size\n");
			else if (result == -2)
				printf("Out of space\n");
		} 
		else if (strcmp(buf, "remove") == 0) { 
			scanf("%s %d", buf, &i);
			result = remove_file(sys, buf, i);   
			if (result == 0) 
				printf("file %s shortened\n", buf);
			else if (result == -1)
				printf("Error: invalid file or size\n");
		}                               
		else if (strcmp(buf, "delete") == 0) {  
			scanf("%s", buf);  /* read the name */
			result = delete_node(sys, buf);   
			if (result == 0) 
				printf("%s deleted\n", buf);
			else if (result == -1)
				printf("Error: invalid dir or file name\n");  
			else if (result == -2)
				printf("Directory is not empty\n");
		}   
		else if (strcmp(buf, "prfiles") == 0) {
			prfiles(sys->root);
		}
		else if (strcmp(buf, "prdisk") == 0) {
			prdisk(sys);
		}
		else {
			printf("%s: invalid command\n", buf);
		}
	}
}


/* destroy the file system */
void destroy_fs(FileSys* sys) {
	LdiskNode* tmp;

	delete_tnode(sys->root);
	
	while (sys->ldisk != NULL) {
		tmp = sys->ldisk;
		sys->ldisk = tmp->next;
		free(tmp);
	}
	free(sys);
}
   
/* create a new directory in current directory, return 0=okay
   return -1=duplicate name, -2=invalid dirname */
int mkdir(FileSys* sys, char* dname) {
	int i;
	TreeNode* t;
	
	/* cannot create another root node */
	if (sys->current == NULL && sys->root != NULL) {
		return -2;
	}
		
		
	if (sys->current == NULL) {
		sys->root = create_tnode(dname, 0);
		if (sys->root == NULL)
			return -2;
	}
	else {               
		i = findchild(sys->current, dname);
		if (i >= 0) {   		
			return -1;
		}
		t = create_tnode(dname, 0);
		if (t == NULL)
			return -2;
		addchild(sys->current,  t);
	}

	return 0;
}

/* set specified directory as the current directory, if dname is ".." 
   set parent directory as current directory, if dname is "/"
   set root directory as current directory
   return 0=okay, -1=failed */
int cd(FileSys* sys, char* dname) {  
	char buf[1024];
	int firstroot = 0;
	TreeNode* next = NULL;
	char* tmp;
	int i;

	if (strcmp(dname, "/") == 0) {
		sys->current = sys->root;
	}
	else if (strcmp(dname, "..") == 0) {
		if (sys->current != NULL && sys->current->parent != NULL)
			sys->current = sys->current->parent;
	}
	else {
		/* if first letter is /, start from root */
		firstroot = (dname[0] == '/' || dname[0] == '\\');
		if (firstroot)
			next = NULL;
		else
			next = sys->current;
		
		strcpy(buf, dname);	
		tmp = strtok(buf, "/\\");
		while (tmp != NULL) {
			if (next == NULL) {
				if (sys->root != NULL && strcmp(tmp, sys->root->name) == 0)
					next = sys->root;
				else
					return -1;
			}
			else {
				/* search in the directory */
				i = findchild(next, tmp);
				
				if (i < 0 || next->children[i]->type == 1)
					return -1;
					
				next = next->children[i];
			}
			tmp = strtok(NULL, "/\\");
		}
		if (next == NULL)
			return -1;
		sys->current = next;
	}
	return 0;
}

/* Print out the directory structure in breadth-first order */
void dir(FileSys* sys) {
	/* use a queue */  
	int qsize = 20;
	TreeNode** queue = (TreeNode**)malloc(sizeof(TreeNode*) * qsize);
	TreeNode** tmp;
	int front = 0;
	int num = 0, i;
	TreeNode* t;
	
	if (sys->root != NULL) {
		queue[0] = sys->root;
		num = 1;
	}
	
	while (num > 0) {
		/* pop the node */
		t = queue[front];
		front = (front + 1) % qsize;
		num--;
		
		/* print the node */
		if (t->type == 0) {
			print_dir(t);
			printf("\n");
		}
		else {
			print_dir(t->parent);
			printf("%s\n", t->name);
		}
		
		/* increase the queue size  */
		if (num + t->childsize > qsize) {  			
			tmp = (TreeNode**)malloc(sizeof(TreeNode*) * (num + t->childsize));
			for (i = 0; i < num; i++) {
				tmp[i] = queue[(front+i)%qsize];
			}
			qsize = num + t->childsize;
			front = 0;
			free(queue);
			queue = tmp;
		}
		for (i = 0; i < t->childsize; i++) {
			queue[(front+num)%qsize] = t->children[i];
			num++;
		}	
	}	
	free(queue);
}

/* create the file in the current directoy, fsize is the number of bytes 
 return 0=okay, -1=duplicate name, -2=invalid name or size, -3=no spaces
  -4=invalid current directory
*/
int create_file(FileSys* sys, char* fname, int fsize) { 
	TreeNode* t = NULL;
	int i;
	if (sys->numfree == 0)
		return -3;
	if (sys->current == NULL)
		return -4;		
	if (fsize < 0)
		return -2;
		             
	i = findchild(sys->current, fname);
	if (i >= 0) {   		
		return -1;
	}
	
	t = create_tnode(fname, 1);
	sys->current->timest = time(0);
	addchild(sys->current, t);
	
	/* appended byte */
	if (fsize > 0)
		return append_file(sys, fname, fsize);
	
	return 0; 		
} 

/* append the number of bytes(fsize) for the file
 return 0=okay, -1=invalid name or size, -2=no spaces
 */
int append_file(FileSys* sys, char* fname, int fsize) {   
	int i;
	int numfnode = 0;
	int reqblk = 0;
	int reqsize = 0;
	int blkid;
	TreeNode* tnode;
	LfileNode* lastf = 0;
	LfileNode* tmp;
	 
	if (sys->current == NULL || fsize < 0)
		return -1;		
	i = findchild(sys->current, fname);
	if (i < 0) {   		
		return -1;
	}
	tnode = sys->current->children[i];
	/* the node is directory? */
	if (tnode->type == 0)
		return -1;

	/* calculate number of node, size required  */	                                  
	numfnode = flist_size(tnode->lfile);
	reqsize = tnode->size + fsize - numfnode * sys->blocksz;  
	
	/* determine if there are enough free blocks */
	if (reqsize > sys->blocksz * sys->numfree)
		return -2;
		        
	/* update timestamp */
	tnode->timest = time(0);
	tnode->size += fsize;
	
	/* there is enough space */
	if (reqsize <= 0) 
		return 0;
					 
	/* calculate block required */
	reqblk = reqsize / sys->blocksz;
	if (reqblk * sys->blocksz < reqsize)
		reqblk++;
		
	lastf = last_filenode(tnode->lfile);
	
	while (reqblk > 0) {                              
		 /* the alloc_block always succeed, because we've already
		   checked the number of free blocks using sys->numfree */
		blkid = alloc_block(sys->ldisk);
		
		/* add the block to the end of Lfile list */
		tmp = create_filenode(blkid * sys->blocksz);
		if (lastf == NULL)
			tnode->lfile = tmp;
		else
			lastf->next = tmp;
		lastf = tmp;		
	
		reqblk--;
		sys->numfree--;
	}

	return 0;
} 

/* remove the number of bytes for the file
 return 0=okay, -1=invalid name or size */
int remove_file(FileSys* sys, char* fname, int fsize) {  
	TreeNode* tnode; 
	LfileNode* tmp; 
	LfileNode* curr;   
	LfileNode* last;
	int i;
	int newblk;

	if (sys->current == NULL || fsize < 0)
		return -1;		
	i = findchild(sys->current, fname);
	if (i < 0) {   		
		return -1;
	}   
	tnode = sys->current->children[i];
	if (tnode->type == 0)
		return -1;
	if (fsize > tnode->size)
		return -1;
	
	/* update time stamp */	             
	sys->current->timest = time(0);   
	tnode->timest = sys->current->timest;
	
	tnode->size -= fsize;
	                       
	/* calculate new block number */
	newblk = tnode->size / sys->blocksz;
	if (newblk * sys->blocksz < tnode->size)
		newblk++;	
		
	if (newblk == 0) {
		curr = tnode->lfile;
		tnode->lfile = NULL;
	}
	else {  
		/* move to new last block */	
		last = tnode->lfile;
		for (i = 1; i < newblk; i++)
			last = last->next;
		curr = last->next;
		last->next = NULL;
	}
	
	/* free block after curr */
	while (curr != NULL) {  
		free_block(sys->ldisk, curr->address / sys->blocksz); 
		sys->numfree++;
		
		tmp = curr;
		curr = curr->next;
		free(tmp);
	}
	
	return 0;
}			

/* delete the node from the file system
 return 0=okay, -1=invalid name , -2=directoy not empty*/
int delete_node(FileSys* sys, char* name) {  
	TreeNode* tnode; 
	LfileNode* tmp;
	int i;

	if (sys->current == NULL)
		return -1;		
	i = findchild(sys->current, name);
	if (i < 0) {   		
		return -1;
	}
	tnode = sys->current->children[i];
	if (tnode->type == 0 && tnode->childsize > 0)
		return -2;
		
	/* remove from parent */
	while (i+1 < sys->current->childsize) {
		sys->current->children[i] = sys->current->children[i+1];
		i++;
	}
	sys->current->childsize--;
	sys->current->timest = time(0);
	
	for (tmp = tnode->lfile; tmp != NULL; tmp = tmp->next) {
		free_block(sys->ldisk, tmp->address / sys->blocksz);
		sys->numfree++;
	}
	delete_tnode(tnode);
	
	return 0;
}   

/* get total size of the files */
int total_fsize(TreeNode* node) {
	int sz = 0, i;
	if (node == NULL)
		return 0;
	
	if (node->type == 1)
		sz = node->size;
	else {
		for (i = 0; i < node->childsize; i++)
			sz += total_fsize(node->children[i]);
	}	
	return sz;
}  

/* display the file info */
void prfiles(TreeNode* node) { 
	int i;
	char buf[128];
	struct tm* tinfo;
	LfileNode* lfile;
	if (node == NULL)
		return;
	
	if (node->type == 1) {
		printf("---------------------------------------------\n"); 
		print_dir(node->parent);
		printf("%s [%d bytes, last modifed: ", node->name, node->size);
		
		tinfo = localtime(&(node->timest));
		strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tinfo);
		printf("%s]\n", buf);
		lfile = node->lfile;
		while (lfile != NULL) {
			if (lfile != node->lfile)
				printf(" -> ");
			printf("%d", lfile->address);
			lfile = lfile->next;
		}
		printf("\n\n");
	}
	else {  
		for (i = 0; i < node->childsize; i++)
			prfiles(node->children[i]);
	}    
}
	
/* display the disk info */
void prdisk(FileSys* sys) { 
	int totalfsize = total_fsize(sys->root);
	int blkused = sys->numblk - sys->numfree;
	LdiskNode* disk = sys->ldisk;
	
	while (disk != NULL) {
		if (disk->used == 1)
			printf("In used: %d-%d\n", disk->first, disk->last);
		else
			printf("Free: %d-%d\n", disk->first, disk->last);
		disk = disk->next;
	}
	printf("fragmentation: %d bytes\n", blkused * sys->blocksz - totalfsize); 
}


