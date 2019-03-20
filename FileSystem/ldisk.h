#ifndef LDISK_H
#define LDISK_H

typedef struct _LdiskNode {
	int first;  /* first id */
	int last;   /* last id */
	int used;   /* 0=free 1=used */
	
	struct _LdiskNode* next;
} LdiskNode;

/* create new disk node */
LdiskNode* create_disknode(int first, int last, int used);

/* split the disk node, return -1 if failed 0=okay */
int split_disknode(LdiskNode* node, int newfirst);

/* merge the disk node */
void merge_disknode(LdiskNode* node);

/* allocate free block from the disk , return 
  the result block ID or -1 if no free block */
int alloc_block(LdiskNode* disk);

/* free the block */
void free_block(LdiskNode* disk, int blkid);

#endif

