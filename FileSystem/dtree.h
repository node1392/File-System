#ifndef DTREE_H
#define DTREE_H
				    
#include <time.h>
#include "lfile.h"

typedef struct _TreeNode {
	struct _TreeNode** children;  /* each node contains several children */
	int childsize;                /* number of children */
	int childcapacity;            /* capacity of the children array */
	struct _TreeNode* parent;     /* parent node */
	
	int type;  /* 0=dir,  1=file */
	char* name;
	time_t timest;  /* time stamp */
	int size;   /* file size */
	
	LfileNode* lfile;  /* pointer to Lfile */ 
} TreeNode;

/* allocate and init a new tree node, return NULL if failed */
TreeNode* create_tnode(char* nm, int type);

/* recursively destroy the tree node */
void delete_tnode(TreeNode* node);

/* find child by name, return index or -1 if not found */
int findchild(TreeNode* parent, char* name);

/* add child to the parent, return 0 if ok and -1 if failed */
int addchild(TreeNode* parent, TreeNode* child);



#endif

