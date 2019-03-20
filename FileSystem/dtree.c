/*
 Implement the dtree data structure
 */

#include <stdio.h>
#include <stdlib.h>   
#include <string.h> 

#include "dtree.h"




/* allocate and init a new tree node */
TreeNode* create_tnode(char* nm, int type) {
	TreeNode* ts = NULL;
	int i; 

	if (strlen(nm) == 0)
		return NULL;
		
	for (i = 0; nm[i]; i++) {
		if (nm[i] == '/' || nm[i] == '\\')
			return NULL;
	}
	
	ts = (TreeNode*)malloc(sizeof(TreeNode));
	ts->children = NULL;
	ts->childsize = 0;
	ts->childcapacity = 0;
	ts->parent = NULL;
	
	ts->type = type;
	ts->timest = time(0);
	ts->size = 0;
	ts->name = (char*)malloc(strlen(nm) + 1);
	strcpy(ts->name, nm);
	
	ts->lfile = NULL;	
	
	return ts;
}
	 
/* recursive destroy the tree node */
void delete_tnode(TreeNode* node) {
	int i;
	LfileNode* tmp;
	if (node == NULL)
		return;
	
	for (i = 0; i < node->childsize; i++)
		delete_tnode(node->children[i]);
	free(node->name);
	free(node->children);
	
	while (node->lfile != NULL) {
		tmp = node->lfile;
		node->lfile = tmp->next;
		free(tmp);
	}
	free(node);
}

/* find child by name */
int findchild(TreeNode* parent, char* name) {
	int i;
	if (parent == NULL)
		return -1;
	for (i = 0; i < parent->childsize; i++) {
		if (strcmp(parent->children[i]->name, name) == 0)
			return i;
	}
	return -1;
} 

/* add child to the parent, return 0 if ok and -1 if failed */
int addchild(TreeNode* parent, TreeNode* child) {
	TreeNode** tmp;
	int i;
	
	if (parent == NULL || child == NULL)
		return -1;
	if (parent->type == 1)
		return -1;
	
	if (findchild(parent, child->name) >= 0)
		return -1;
	
	/* grow the array if it is full */	
	if (parent->childsize >= parent->childcapacity) {
		parent->childcapacity = (int)(parent->childcapacity * 1.3) + 2;
		tmp = (TreeNode**)malloc(sizeof(TreeNode*) * parent->childcapacity);
		for (i = 0; i < parent->childsize; i++)
			tmp[i] = parent->children[i];
		free(parent->children);
		parent->children = tmp;
	}
	
	parent->children[ parent->childsize ] = child;
	child->parent = parent;
	parent->childsize++;
	
	return 0;
}


