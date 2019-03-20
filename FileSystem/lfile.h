#ifndef LFILE_H
#define LFILE_H

typedef struct _LfileNode {
	int address;   /* ID * block_size */	
	struct _LfileNode* next;
}LfileNode;

/* create new file node */  
LfileNode* create_filenode(int addr);

/* get the list size */
int flist_size(LfileNode* node);

/* get the last node */
LfileNode* last_filenode(LfileNode* node);

#endif

