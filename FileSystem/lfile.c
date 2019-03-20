/*
 Implement the file linked list
 */

#include <stdio.h>
#include <stdlib.h>   
#include <string.h>   

#include "lfile.h"
					  
/* create new file node */  
LfileNode* create_filenode(int addr) {
	LfileNode* node = (LfileNode*)malloc(sizeof(LfileNode));
	node->address = addr;
	node->next = NULL;
	return node;
}

/* get the list size */
int flist_size(LfileNode* node) {
	int sz = 0;
	while (node != NULL) {
		sz++;
		node = node->next;
	}
	return sz;
}

/* get the last node */
LfileNode* last_filenode(LfileNode* node) {
	while (node != NULL && node->next != NULL)
		node = node->next;
	return node;
}



