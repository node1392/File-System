/*
 Implement the disk linked list data structure
 */

#include <stdio.h>
#include <stdlib.h>   
#include <string.h> 

#include "ldisk.h"


/* create new disk node */
LdiskNode* create_disknode(int first, int last, int used) {  
	LdiskNode* node = (LdiskNode*)malloc(sizeof(LdiskNode));
	node->first = first;
	node->last = last;
	node->used = used;
	node->next = NULL;
	return node;
}
	 
/* split the disk node, return -1 if failed 0=okay */
int split_disknode(LdiskNode* node, int newfirst) { 
	LdiskNode* tmp;
	if (node == NULL || newfirst <= node->first || newfirst > node->last)
		return -1;
		 
	tmp = create_disknode(newfirst, node->last, node->used);
	node->last = newfirst-1;
	tmp->next = node->next;
	node->next = tmp;
		
	return 0;
}
	
/* merge the disk node */
void merge_disknode(LdiskNode* node) {   
	LdiskNode* tmp;
	while (node != NULL && node->next != NULL && 
		node->next->used == node->used) {
		tmp = node->next;
		node->next = tmp->next;
		node->last = tmp->last;
		free(tmp);
	}			
}

/* allocate free block from the disk , return the result block ID 
  or -1 if no free block */
int alloc_block(LdiskNode* disk) {
	LdiskNode* prev = 0;
	LdiskNode* curr = disk;
	LdiskNode* tmp;
	int blkid = -1;
	
	while (curr != NULL) {
		if (curr->used == 0) {
			/* find a free disk block */
			blkid = curr->first; 
			
			if (curr->first != curr->last) {
				/* split block */
				split_disknode(curr, curr->first + 1);
			}
			       			                               
			curr->used = 1;
			/* try to merge */
			if (prev != NULL) {
				curr = prev;
			}
			merge_disknode(curr);
			
			return blkid;
		}
	
		prev = curr;
		curr = curr->next;
	} 	
	
	return -1;	
}   

/* free the block */
void free_block(LdiskNode* disk, int blkid) {  
	LdiskNode* prev = 0;
	LdiskNode* curr = disk; 
	LdiskNode* tmp;
	
	while (curr != NULL) {
		if (blkid >= curr->first && blkid <= curr->last) {
			/* split the blocks before blkid */  
			if (blkid > curr->first) {  
				split_disknode(curr, blkid);  			 
				
				prev = curr;
				curr = curr->next;  /* move to the new node */
			}
			
			/* split the blocks after blkid */
			if (curr->first != curr->last) {   
				split_disknode(curr, curr->first + 1);
			}
			curr->used = 0;
			                   
			/* try to merge */
			if (prev != NULL && prev->used == curr->used) {
				curr = prev;
			}
			merge_disknode(curr);
			
			return;
		}
		          
		prev = curr;
		curr = curr->next;
	}
}




