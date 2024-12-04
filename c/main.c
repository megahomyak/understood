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

#define vec(type) /* semantic marker */ type*

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

void vec_free(void* data) {
    if (data != NULL) free(data - _VEC_DATA_BIAS);
}

#define vec_push(data, value) (_vec_prepare_for_push((void**)&data, sizeof(*data)) == RESULT_SUCCESS ? ((data[_vec_valid_len(data - _VEC_DATA_BIAS)++] = value), RESULT_SUCCESS) : RESULT_FAILURE)
#define vec_pop(data) data[(_vec_valid_len(data)--) - 1]
#define VEC_EMPTY NULL

/* Option utils. Prefix: "option"
*/

typedef enum {
    OPTION_NONE,
    OPTION_SOME,
} OptionStatus;

#define option(type) struct { OptionStatus status; type some; }

#define EMPTY_OPTION { .status = OPTION_NONE }

#define option_set(opt, value) (opt.status = OPTION_SOME, opt.some = value)
#define option_is_set(opt) (opt.status == OPTION_SOME)

/* Runtime utils. Prefix: "runtime"
*/

void runtime_die(void) {
    exit(1);
}
#define runtime_check(expr) if (expr == RESULT_FAILURE) runtime_die()

/* Parser utils. Prefix: "parser"
*/

typedef enum {
    PARSER_TEXT,
    PARSER_GROUP,
} ParserNodeType;

typedef size_t ParserIndex;

typedef vec(struct ParserNode) ParserGroup;
#define _PARSER_EMPTY_GROUP VEC_EMPTY
typedef struct {
    char first;
    vec(char) rest;
} ParserText;

#define _parser_char_not_null char

Result _parser_text_push(ParserText* text, _parser_char_not_null c) {
    if (text->first == '\0') {
        text->first = c;
        return RESULT_SUCCESS;
    } else {
        return vec_push(text->rest, c);
    }
}

#define _PARSER_EMPTY_TEXT ((ParserText){.first='\0', .rest=VEC_EMPTY})

#define _parser_text_is_empty(text) (text.first == '\0')

typedef struct {
    ParserText text;
    ParserGroup group;
} ParserNodeTypeData;

typedef struct ParserNode {
    ParserNodeType type;
    ParserIndex idx;
    ParserNodeTypeData data;
} ParserNode;

typedef struct {
    ParserGroup root;
    vec(ParserIndex) unexpected_closers;
    vec(ParserIndex) unclosed_openers;
} ParserResult;

void parser_parse(ParserResult* result, char* input) {
    ParserGroup root = _PARSER_EMPTY_GROUP;
    typedef struct {
        ParserIndex idx;
        ParserGroup group;
    } Overlay;
    vec(Overlay) overlays = VEC_EMPTY;

    ParserIndex idx = 0;

    ParserText sbuf = _PARSER_EMPTY_TEXT;
    ParserIndex sbufidx = idx;

    vec(ParserIndex) unclosed_openers = VEC_EMPTY;
    vec(ParserIndex) unexpected_closers = VEC_EMPTY;

    bool escaped = false;

    for (;;) {
        ParserIndex curidx = idx;
        char c = input[idx];

        if (c != '\0') {
            ++idx;
            if (escaped) {
                if (!(c == '(' || c == ')' || c == '\\')) {
                    runtime_check(_parser_text_push(&sbuf, '\\'));
                }
                runtime_check(_parser_text_push(&sbuf, c));
                escaped = false;
                continue;
            }
        }

        if (c == '\0' || c == '(' || c == ')') {
            option(Overlay) finished_layer = EMPTY_OPTION;
            if (c == '\0' || c == ')') {
                if (vec_len(overlays) == 0) {
                    if (c != '\0') {
                        runtime_check(vec_push(unexpected_closers, curidx));
                    }
                } else {
                    option_set(finished_layer, vec_pop(overlays));
                }
            }
            ParserGroup* top;
            if (vec_len(overlays) == 0) {
                top = &root;
            } else {
                top = &overlays[vec_len(overlays) - 1].group;
            }
            if (!_parser_text_is_empty(sbuf)) {
                ParserNode text = {
                    .type = PARSER_TEXT,
                    .data.text = sbuf,
                    .idx = sbufidx,
                };
                runtime_check(vec_push(*top, text));
                sbuf = _PARSER_EMPTY_TEXT;
                sbufidx = idx;
            }
            if (option_is_set(finished_layer)) {
                ParserIndex opener_idx = finished_layer.some.idx;
                ParserNode group = {
                    .type = PARSER_GROUP,
                    .idx = finished_layer.some.idx,
                    .data.group = finished_layer.some.group,
                };
                runtime_check(vec_push(*top, group));
                if (c == '\0') {
                    runtime_check(vec_push(unclosed_openers, opener_idx));
                    continue;
                }
            }
            if (c == '\0') {
                break;
            }
            if (c == '(') {
                Overlay overlay = {
                    .idx = curidx,
                    .group = _PARSER_EMPTY_GROUP,
                };
                runtime_check(vec_push(overlays, overlay));
            }
        } else {
            if (c == '\\') {
                escaped = true;
            } else {
                runtime_check(_parser_text_push(&sbuf, c));
            }
        }
    }

    result->root = root;
    result->unclosed_openers = unclosed_openers;
    result->unexpected_closers = unexpected_closers;

    vec_free(overlays);
}

void parser_free_group(ParserGroup group) {
    typedef struct {
        ParserNodeType type;
        ParserNodeTypeData data;
    } Task;
    vec(Task) tasks = VEC_EMPTY;
    {
        Task root = {
            .type = PARSER_GROUP,
            .data.group = group,
        };
        runtime_check(vec_push(tasks, root));
    }
    while (vec_len(tasks) != 0) {
        Task task = vec_pop(tasks);
        switch (task.type) {
            case PARSER_TEXT:
                vec_free(task.data.text.rest);
                break;
            case PARSER_GROUP:
                for (size_t i = 0; i < vec_len(task.data.group); i++) {
                    ParserNode node = task.data.group[i];
                    Task task = {
                        .type = node.type,
                        .data = node.data,
                    };
                    runtime_check(vec_push(tasks, task));
                }
                vec_free(task.data.group);
                break;
        }
    }
}

int main(void) {
    ParserResult result;
    parser_parse();
    return 0;
}
