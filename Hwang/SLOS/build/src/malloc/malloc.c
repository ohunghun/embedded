#define ALIGNMENT 2
#define HDSIZE ALIGN(sizeof(block))
#define ALIGN(p) (((int)(p) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

#define NULL 0
#define BYTE	unsigned char
#include "malloc.h"
void* heapadd;
void* heapend;
void testinit();
BYTE term;
int boundary_min;
int boundary_max;
t table;
block* free_list;

block* ex_find_fit(int size);
block* heap_address();
void* extend_heap(int size);


void mallocinit()
{
	heapadd = heap_address();
	heapend=(char*)heapadd+HDSIZE;
	block* b=(block*)heapadd;
	b->next=b;
	b->pre=b;
	b->size=1;
	b->_real_size=0;
	
}

void* malloc(int size)
{
	block* fb;
	int asize=ALIGN(size);
	block* bp=ex_find_fit(asize+HDSIZE);

	if(bp==NULL)
	{
		bp=(block*)extend_heap(asize+HDSIZE);
		bp->size=asize |1;
		bp->next=NULL;
		bp->pre=NULL;
		bp->_real_size=size;
		return (char*)(bp)+HDSIZE;
	}
	else
	{
		if(bp->size> asize+HDSIZE+HDSIZE)
		{

			fb=(block*)( (char*)(bp)+HDSIZE+asize );
			fb->size=bp->size-asize-HDSIZE&~1;
			bp->size=asize|1;
			bp->pre->next=fb;
			fb->pre=bp->pre;
			fb->next=bp->next;
			bp->next->pre=fb;
			block* tt=(block*)heap_address();
			bp->next=NULL;
			bp->pre=NULL;
			bp->_real_size=size;
			return (char*)(bp)+HDSIZE;
		}
		bp->size|=1;
		bp->pre->next=bp->next;
		bp->next->pre=bp->pre;

		bp->next=NULL;
		bp->pre=NULL;
		bp->_real_size=size;
		return (char*)(bp)+HDSIZE;
	}
}

block* ex_find_fit(int size)
{
	block* p;

	for(p=heap_address()->next;p != heap_address()&&p->size < size ;p=p->next);

	if(p!=heap_address())
		return p;
	else 
		return NULL;
}




void free(void* ptr)
{
	if(ptr==0)
		return;
	block* bp=((block*)((char*)ptr-HDSIZE));
	block* hl=(block*)heap_address();
	block* t=hl->next;
	hl->next=bp;
	bp->next=t;
	t->pre=bp;
	bp->pre=hl;
	bp->size=bp->size&~1;
	bp->_real_size=0;
}
void* extend_heap(int size)
{
	//void* temp=heapend;
	heapend=(char*)heapend+size;
	return (char*)heapend-size;
}
block* heap_address()
{
	//return 0xA1500000;
	return 0xA2600000;
}

