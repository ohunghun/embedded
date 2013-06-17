#define TABLE_SIZE 400

typedef struct block{
	int size;
	int _real_size;
	struct block *next;
	struct block *pre;
}block;
typedef struct t{
	int size;
	block* head[TABLE_SIZE];
}t;

void* malloc(int size);
void free(void* );

void mallocinit(void);
block* heap_address();
