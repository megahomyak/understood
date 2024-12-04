#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Result utils. Prefix: "result"
*/

typedef enum {
    RESULT_SUCCESS,
    RESULT_FAILURE,
} Result;

/* Vec utils. Prefix: "vec"
Vec layout: len,cap,[data]
Pointer starting at "len" is called "root"
Pointer starting at "data" is called "data"
*/

#define vec(type) /* semantic marker */ type
#define vec_not_null(type) /* semantic marker */ type

#define _VEC_DATA_BIAS sizeof(size_t)*2

#define _vec_valid_len(root) ((size_t*)root)[0]
#define _vec_valid_cap(root) ((size_t*)root)[1]

size_t vec_len(void* data) {
    return data == NULL ? 0 : _vec_valid_len(data - _VEC_DATA_BIAS);
}

#define _VEC_GROWTH_FACTOR 2
#define _VEC_INITIAL_CAPACITY 8

Result _vec_prepare_for_push(void** result, size_t item_size) {
    void* given_ptr = *result;
    size_t capacity;
    if (given_ptr == NULL) {
        capacity = _VEC_INITIAL_CAPACITY;
    } else {
        given_ptr = given_ptr - _VEC_DATA_BIAS;
        if (_vec_valid_len(given_ptr) == _vec_valid_cap(given_ptr)) {
            // Not enough space, time to allocate more
            capacity = _vec_valid_cap(given_ptr)*_VEC_GROWTH_FACTOR;
            if (capacity / _VEC_GROWTH_FACTOR != _vec_valid_cap(given_ptr)) {
                // Overflow
                return RESULT_FAILURE;
            }
        } else {
            // Enough space for now, no need to allocate more
            return RESULT_SUCCESS;
        }
    }
    size_t data_size = item_size * capacity;
    if (data_size / item_size != capacity) {
        // Overflow
        return RESULT_FAILURE;
    }
    size_t total_size = data_size + _VEC_DATA_BIAS;
    if (total_size < data_size) {
        // Overflow
        return RESULT_FAILURE;
    }
    void* alloc_ptr = realloc(given_ptr, total_size);
    if (alloc_ptr != NULL) {
        if (given_ptr == NULL) {
            _vec_valid_len(alloc_ptr) = 0;
        }
        _vec_valid_cap(alloc_ptr) = capacity;
        alloc_ptr += _VEC_DATA_BIAS;
        *result = alloc_ptr;
        return RESULT_SUCCESS;
    } else {
        return RESULT_FAILURE;
    }
}

void vec_dealloc(void* data) {
    if (data != NULL) free(data - _VEC_DATA_BIAS);
}

void vec_dealloc_anyway(void* data) {
    free(data - _VEC_DATA_BIAS);
}

#define vec_push(data, value) (_vec_prepare_for_push((void**)&data, sizeof(*data)) == RESULT_SUCCESS ? ((data[_vec_valid_len(data - _VEC_DATA_BIAS)++] = value), RESULT_SUCCESS) : RESULT_FAILURE)
#define vec_pop(data) ptr[(_valid_len(ptr)--) - 1]
#define VEC_EMPTY NULL

/* Parser utils. Prefix: "parser"
*/

typedef enum {
    PARSER_TEXT,
    PARSER_GROUP,
} ParserNodeType;

typedef size_t ParserIndex;

typedef vec(struct ParserNode*) ParserGroup;

typedef struct ParserNode {
    ParserNodeType type;
    union {
        vec_not_null(char*) text;
        ParserGroup group;
    };
} ParserNode;

typedef struct {
    ParserGroup root;
    vec(ParserIndex*) unexpected_closers;
    vec(ParserIndex*) unclosed_openers;
} ParserResult;

void parser_nodes_cleanup(ParserGroup* nodes) {
    vec(ParserNode*) tasks = VEC_EMPTY;
}

Result parser_parse(ParserResult* result, char* input) {
    ParserGroup root = VEC_EMPTY;
    typedef struct {
        ParserIndex idx;
        ParserGroup group;
    } Overlay;
    vec(Overlay*) overlays = VEC_EMPTY;

    ParserIndex idx = 0;

    vec(char*) sbuf = VEC_EMPTY;
    ParserIndex sbufidx = idx;

    vec(ParserIndex*) unclosed_openers = VEC_EMPTY;
    vec(ParserIndex*) unexpected_closers = VEC_EMPTY;

    bool escaped = false;

    for (;;) {
        ParserIndex curidx = idx;
        char c = input[idx];

        if (c != '\0') {
            ++idx;
            if (escaped) {
                if (vec_push(sbuf, c) == RESULT_FAILURE) goto die;
            }
        }
    }

    result->root = root;
    result->unclosed_openers = unclosed_openers;
    result->unexpected_closers = unexpected_closers;

    die:
    parser_nodes_cleanup(root);
    return RESULT_FAILURE;
}
