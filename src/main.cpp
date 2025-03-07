#include <assert.h>
#include <cstring>
#include <iostream>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT sizeof(void *)
#endif

typedef struct Pool_Free_Node Pool_Free_Node;
struct Pool_Free_Node {
	Pool_Free_Node *next;
};

typedef struct Pool Pool;
struct Pool {
	unsigned char *buffer;
	size_t length;
	size_t chunk_size;

	Pool_Free_Node *head;
};

bool is_power_of_two(size_t x) {
	return (x & (x - 1)) == 0;
}

uintptr_t align_uintptr(uintptr_t ptr, size_t align) {
	assert(is_power_of_two(align));

	uintptr_t p, a, modulo;

	p = ptr;
	a = (uintptr_t)align;
	modulo = p & (a - 1);

	if (modulo != 0) {
		p += a - modulo;
	}

	return p;
}

size_t align_size(size_t size, size_t align) {
	assert(is_power_of_two(align));

	size_t s, a, modulo;

	s = (size_t)size;
	a = align;
	modulo = s & (a - 1);

	if (modulo != 0) {
		s += a - modulo;
	}

	return s;
}

void pool_free_all(Pool *p) {
	size_t chunk_count = p->length / p->chunk_size;

	for (size_t i = 0; i < chunk_count; ++i) {
		void *ptr = &p->buffer[i * p->chunk_size];
		Pool_Free_Node *node = (Pool_Free_Node *)ptr;
		node->next = p->head;
		p->head = node;
	}
}

void pool_init(Pool *p, void *buffer, size_t length, size_t chunk_size, size_t chunk_alignment) {
	chunk_size = align_size(chunk_size, chunk_alignment);
	assert(chunk_size >= sizeof(Pool_Free_Node) && "Chunk size is too small!");

	uintptr_t initial_start = (uintptr_t)buffer;
	uintptr_t start 		= align_uintptr(initial_start, chunk_alignment);
	length 				   -= (size_t)(start - initial_start);
	length 					= (length / chunk_size) * chunk_size;
	
	assert(length >= chunk_size && "Buffer length is smaller than chunk size!");

	p->buffer 		= (unsigned char *)start;
	p->length 		= length;
	p->chunk_size 	= chunk_size;
	p->head 		= NULL;

	pool_free_all(p);
}

void *pool_alloc(Pool *p) {
	void *ptr = p->head;

	if (ptr == NULL) {
		assert(0 && "Pool has no free memory!");
		return NULL;
	}

	p->head = p->head->next;

	return memset(ptr, 0, p->chunk_size);
}

void pool_free(Pool *p, void *ptr) {
	if (ptr == NULL) {
		return;
	}

	if (!(&p->buffer <= ptr && ptr < &p->buffer[p->length])) {
		assert(0 && "Memory is outside pool buffer!");
		return;
	}

	Pool_Free_Node *node = (Pool_Free_Node *)ptr;
	node->next = p->head;
	p->head = node;
}

int main() {
	int i;
	unsigned char backing_buffer[1024];
	Pool p;
	void *a, *b, *c, *d, *e, *f;
	
	pool_init(&p, backing_buffer, 1024, 64, DEFAULT_ALIGNMENT);
	
	a = pool_alloc(&p);
	b = pool_alloc(&p);
	c = pool_alloc(&p);
	d = pool_alloc(&p);
	e = pool_alloc(&p);
	f = pool_alloc(&p);
	
	*(int *)a = 5;
	*(int *)b = 10;
	*(int *)c = 15;
	*(int *)d = 20;
	*(int *)e = 25;
	*(int *)f = 30;
	
	//All ints
	std::cout << *(int *)a << std::endl;
	std::cout << *(int *)b << std::endl;
	std::cout << *(int *)c << std::endl;
	std::cout << *(int *)d << std::endl;
	std::cout << *(int *)e << std::endl;
	std::cout << *(int *)f << std::endl;
	
	pool_free(&p, b);
	pool_free(&p, c);
	pool_free(&p, d);
	pool_free(&p, f);
	
	//b, c, d, f have been replaced by Pool_Free_Nodes
	std::cout << *(int *)a << std::endl;
	std::cout << *(int *)b << std::endl;
	std::cout << *(int *)c << std::endl;
	std::cout << *(int *)d << std::endl;
	std::cout << *(int *)e << std::endl;
	std::cout << *(int *)f << std::endl;
	
	d = pool_alloc(&p);
	
	pool_free(&p, a);
	
	a = pool_alloc(&p);
	
	pool_free(&p, a);
	pool_free(&p, d);
	pool_free(&p, e);
	
	return 0;
}