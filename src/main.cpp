#include <assert.h>
#include <cstring>
#include <iostream>

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

void pool_free_all(Pool *p);

void init_pool(Pool *p, void *buffer, size_t length, size_t chunk_size, size_t chunk_alignment) {
	chunk_size = align_size(chunk_size, chunk_alignment);
	assert(chunk_size >= sizeof(Pool_Free_Node) && "Chunk size is too small");
}



int main() {
	return 1;
}