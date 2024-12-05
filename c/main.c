#include <stdlib.h>

/* Sized array */

size_t get_size(void* arr) {
    return ((size_t*)arr)[-1];
}
void* to_sized(void* ptr, size_t size) {
    *((size_t*)ptr++) = size;
    return ptr;
}
#define sized_array(type) type*

/* Parser */

typedef struct {
    size_t unclosed_openers;
    size_t unexpected_closers;
    size_t rest;
} ParsingSizes;

typedef size_t Index;

typedef struct {
    sized_array(Index) unclosed_openers;
    sized_array(Index) unexpected_closers;
    void* memory;
} ParsingResult;
