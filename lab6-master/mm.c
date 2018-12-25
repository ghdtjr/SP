/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
#define WSIZE 4
#define DSIZE 8 

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/*
   added macro from the book
*/
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define CHUNKSIZE (1<<12)

// Pack a (size and allocated bit) into a word
#define PACK(size, alloc) ((size) | (alloc))

// get or put the value of pointer
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

//size of block p
//allloc'd or not of the block p
//pointer p is must be a head pointer
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

// compute address of its header and footer
#define HDRP(bp)	((char *)(bp) -WSIZE)
#define FTRP(bp)	((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

//compute address of next and previous blocks (not tracing free block)
#define NEXT_BLKP(bp)	((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)	((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

//#define PREV_FBLP(bp)	(*(char **)(bp+4))
//#define NEXT_FBLP(bp)	(*(char **)(bp))

#define SET_PREV_FBLP(bp, qp) (PREV_FBLP(bp) = qp)
#define SET_NEXT_FBLP(bp, qp) (NEXT_FBLP(bp) = qp)

#define GET_4B(p)       (*(unsigned int *)(p))

// my functions

//#include <assert.h>
static void *extend_heap(size_t words);
static void *find_fit(size_t size);
//static void delete(void *bp);
static void place(void* bp, size_t size);
//static void insert(void* bp);
static void *coalesce(void* bp);

/*
   basically we often use the bp which is located after the one word size of the head,
   but we access to the head or tail when we need the size or valid check of the block.
   bp is the pointer which dictates the payroad data directly
*/

static char *heap_listp = 0;
//static char *free_listp = 0;
//static char *tail_listp = 0;
/*
  extend the heap as the given size
*/
/*
static void printheap(void * ptr, int num){ // print heap for debug
	int q;
	for(q = 0; q<=num; q += 4){
		if( q%32 == 0 ) printf("\n%08x(+%4x):", (unsigned int)(ptr + q), q);
	    printf(" %08x", GET_4B(ptr+q) );
	}
	printf("\n");
}
*/
static void *extend_heap(size_t words)
{

	char *bp;
	size_t size;
	//round up to even for the alignmnet
	size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
	
	// mem_sbrk expands the heap (size) bytes
	// checking error
	if ((long) (bp = mem_sbrk(size)) == -1){
		return NULL;
	}
 	
	//set the head and foot to represent not malloc'd
	//and the next block is none
	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
	
	/*printheap(heap_listp, mem_heapsize());
	printf("%p\n", heap_listp);
	printf("%p\n", free_listp);*/

	return coalesce(bp);
	
}

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
	// create inital empty heap
	//checking error
	if ((heap_listp = mem_sbrk(4*WSIZE)) == (void*)-1) {
		return -1;
	}

	//initializing heap 
	PUT(heap_listp, 0);
	PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));
	PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));
	PUT(heap_listp + (3*WSIZE), PACK(0, 1));
	
	heap_listp += (2*WSIZE);
	
	//free_listp = heap_listp-WSIZE;
	// extend empty heap

	if(extend_heap(CHUNKSIZE/WSIZE) == NULL){
	//printheap(heap_listp, mem_heapsize());
		return -1;
	}
    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */

void *mm_malloc(size_t size)
{

	size_t asize;
	char *bp;
	size_t extendsize;

	// reject negative size
	if (size == 0) {
		return NULL;
	}
	
	// fix the memory we actually need memroy for the tags and alignmnet
	// tags + NEXT + PREV + tags
	if (size <= DSIZE) {
		asize = 2*DSIZE;
	}
	else {
		asize = DSIZE * ((size + (DSIZE) +(DSIZE-1)) /DSIZE);
	}
	
	// check first whether the fitted block is exist or not
	if ((bp = find_fit(asize)) != NULL) {
		place(bp, asize);
		return bp;
	}
	// requires more memory
	extendsize = MAX(asize, CHUNKSIZE);
	if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
		return NULL;
	place(bp, asize);
	return bp;
	//printheap(heap_listp, mem_heapsize());
	//assert(0)
}

/*
   fill the block
   change the state
   if the block is bigger than the required memory, then split and insert.
*/

static void place(void *bp, size_t size)
{
	size_t bsize = GET_SIZE(HDRP(bp));

	// block is bigger than required, we need to split (16 is basic needs)
	if (bsize - size >= (2*DSIZE)){
		//size_t remain_size = bsize - size;
	
		// change the state of the malloc'd part block
		PUT(HDRP(bp), PACK(size, 1));
		PUT(FTRP(bp), PACK(size, 1));
	
		//delete(bp);
		
		// change the state of the not malloc'd(remaining) part block
		bp = NEXT_BLKP(bp);
		PUT(HDRP(bp), PACK(bsize - size , 0));
		PUT(FTRP(bp), PACK(bsize - size, 0));
		// insert the remaining part in to the free list
		//insert(bp);
	}
	
	// block is fit, just change the state
	else {		
		PUT(HDRP(bp), PACK(bsize, 1));
		PUT(FTRP(bp), PACK(bsize, 1));
		//delete(bp);
	}
}

/*
	search the block which is
	first fit!!!
*/

static void *find_fit(size_t size)
{
	void* bp;

	// until GET_SIZE > 0, cause the the last block after the extended heap is 0 block
	for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
		if(!GET_ALLOC(HDRP(bp)) && (size <= GET_SIZE(HDRP(bp)))) {
			return bp;
		}
	}
	return NULL;
				
}

/*
   deleting from the free list, we don't have to trace
   -> it means that we get rid of the block and connect the prev and next block
*/

/*static void delete(void *bp)
{
	// PREV - bp - NEXT
	// PREV - bp - NULL
	// NULL - bp - NEXT
	// NULL - bp - NEXT


	if(PREV_FBLP(bp)){
		//printf("%d\n", PREV_FBLP(bp));
		//printf("%d\n", NEXT_FBLP(bp));
		NEXT_FBLP(PREV_FBLP(bp)) = NEXT_FBLP(bp);
	}
	else{
		if (NEXT_FBLP(bp)) {
			free_listp = NEXT_FBLP(bp);
		}
	}
	 //assert(0);

}
*/
/*
 * mm_free - Freeing a block does nothing.
 */
/* for the explicit
void mm_free(void *bp)
{
	size_t bsize = GET_SIZE(HDRP(bp));

	if (!GET_ALLOC(HDRP(bp))) {
		return;
	}

	PUT(HDRP(bp), PACK(bsize, 0));
	PUT(FTRP(bp), PACK(bsize, 0));
	
	coalesce(bp);
	printheap(heap_listp, mem_heapsize());
}
*/

void mm_free(void *bp)
{
	size_t bsize = GET_SIZE(HDRP(bp));
	
	//change the state of the freed block
	PUT(HDRP(bp), PACK(bsize, 0));
	PUT(FTRP(bp), PACK(bsize, 0));
	coalesce(bp);
}

/*
   the bp should not exist in the free lis
   insert at the first place of the list -> faster than the other way
   NULL - temp
   ---> NULL - bp - free_listp
*/

/* for the explicit
static void insert(void *bp)
{	

	//the init case enter this
	if (free_listp == heap_listp) {
		free_listp = bp;
	}

	else {
	void* next = free_listp;

	
	PREV_FBLP(next) = bp;
	NEXT_FBLP(bp) = next;
	//PREV_FBLP(bp) = NULL;
	free_listp = bp;
	}

	
}
*/

/*
   coalesce is divided in to the four cases
*/

static void *coalesce(void *bp)
{
	//assert( bp != NULL);

	size_t size = GET_SIZE(HDRP(bp));
	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
	size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));

	// mallocd - bp - mallocd
	if(prev_alloc && next_alloc) {
		return bp;
	}
	// mallocd - bp - free
	else if (prev_alloc && !next_alloc) {
		size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size, 0));
	}
	// free - bp - malloc
	else if (!prev_alloc && next_alloc) {
		size += GET_SIZE(HDRP(PREV_BLKP(bp)));
		PUT(FTRP(bp), PACK(size, 0));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);

	}
	// free -bp -free
	else {
		size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}
	return bp;
}
	//if (GET_ALLOC(HDRP(PREV_BLKP(bp))) || PREV_BLKP(bp) == bp) {
		/*
		   no empty
		*/
		//assert(0);

		// when we init the malloc, we enter this case
	//	if (GET_ALLOC(HDRP(NEXT_BLKP(bp)))) {
			//insert(bp);
	//		return bp;
		/*
		   only next is empty
		*/

//		else {
			//delete(NEXT_BLKP(bp));
//			size = size + GET_SIZE(HDRP(NEXT_BLKP(bp)));
//			PUT(HDRP(bp), PACK(size, 0));
//			PUT(FTRP(bp), PACK(size, 0));
//		}
//	}
//	else {
	
		/*
		   only previous block is empty
		*/
//		if (GET_ALLOC(HDRP(NEXT_BLKP(bp)))) {
			//delete(PREV_BLKP(bp));
//			size = size + GET_SIZE(HDRP(PREV_BLKP(bp)));
//			bp = PREV_BLKP(bp);

//			PUT(HDRP(bp), PACK(size, 0));
//			PUT(FTRP(bp), PACK(size, 0));
//		}
		/*
		   previous and next block is both empty
		*/
//		else{
			//delete(PREV_BLKP(bp));
			//delete(NEXT_BLKP(bp));

//			size = size + GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
//			bp = PREV_BLKP(bp);

//			PUT(HDRP(bp), PACK(size, 0));
//			PUT(FTRP(bp), PACK(size, 0));
//		}
//	}
	//before we insert we should remove them in the free list.
	//insert(bp);
//	return bp;
/*
void *mm_realloc(void *ptr, size_t size)
{
	size_t original_size = GET_SIZE(HDRP(ptr));
	size_t newsize;


	if (size == 0) {
		mm_free(ptr);
		return NULL;
	}
	// size is normal
	// new is smaller than the original
	else {
		newsize = size + DSIZE;
		if (newsize <= original_size) {
			return ptr;
		}
		//new is bigger than original
		else {
			if (!GET_ALLOC(NEXT_BLKP(ptr)) && (original_size + GET_SIZE(HDRP(NEXT_BLKP(ptr))) >= newsize)) {
				PUT(HDRP(ptr), PACK(original_size + GET_SIZE(HDRP(NEXT_BLKP(ptr))),1));
				PUT(FTRP(ptr), PACK(original_size + GET_SIZE(HDRP(NEXT_BLKP(ptr))),1));
				return ptr;
			}
			else {
				void *new_position = mm_malloc(newsize);
				memcpy(new_position, ptr, newsize);
				mm_free(ptr);
				return new_position;
			}
		}
	}
}
*/
/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */

// ---------------------------------------------------------------------------------------------------------------------------
// original code
/*
void *mm_malloc(size_t size)
{
	int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
	if (p == (void *)-1)
		return NULL;
	 else {
        *(size_t *)p = size;
	    return (void *)((char *)p + SIZE_T_SIZE);
	}
}
*/
/*
void mm_free(void *ptr)
{
}
*/


void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

/*
*/



