#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum Result {
    SUCCESS,
    FAILURE,
};

enum NodeType {
    GROUP,
    TEXT,
};

union NodeData {
    struct Group* group;
    char* text;
};

struct Node {
    enum NodeType type;
    union NodeData data;
};

struct Group {
    size_t length;
    struct Node contents[];
};

/*
STAGE 1:
OUTPUT:
* How many groups are there
* How deep can the stack go

STAGE 2:
REQUIREMENTS:
* How many groups are there
* How deep can the stack go
OUTPUT:
* How long each group is (separately: root node, overlays)
* The length of the entirety of the memory that has to be allocated

STAGE 3:
REQUIREMENTS:
* How long each group is (separately: root node, overlays)
OUTPUT:
* The finished tree (with text lengths gathered directly from the input string)
*/
enum Result parse(struct ParsingResult* parsing_result, char* input) {
    /* Stage 1 */
    size_t overlays_amount = 0;
    size_t maxstack = 0;
    {
        size_t idx = 0;
        bool escaped = false;
        size_t curstack = 0;
        for (;;) {
            char c = input[idx++];
            if (c == '\0') break;
            if (escaped) {
                escaped = false;
                continue;
            }
            if (c == '(') {
                ++overlays_amount;
                ++curstack;
                if (curstack > maxstack) maxstack = curstack;
            } else if (c == ')') {
                if (curstack != 0) --curstack;
            } else if (c == '\\') escaped = true;
        }
    }

    /* Stage 2 */
    size_t root_length = 0;
    size_t* overlay_lengths;
    void** stack;
    size_t tree_mem_usage = 0;
    {
        size_t cur_overlay;
        if (maxstack != 0) {
            overlay_lengths = malloc(overlays_amount * sizeof(size_t) + maxstack * sizeof(void*));
            if (overlay_lengths == NULL) return FAILURE;
            memset(overlay_lengths, 0, overlays_amount * sizeof(size_t));
            cur_overlay = 0;
            stack = (void**) &overlay_lengths[overlays_amount];
        }
        size_t idx = 0;
        size_t escaped = false;
        size_t stacklen = 0;
        bool currently_on_text = false;
        for (;;) {
            char c = input[idx];
            if (c != '\0') {
                ++idx;
                if (escaped) {
                    if (c == '(' || c == ')' || c == '\\') tree_mem_usage += sizeof('\\');
                    tree_mem_usage += sizeof(c);
                    currently_on_text = true;
                    escaped = false;
                    continue;
                }
            }
            if (c == '\0' || c == '(' || c == ')') {
                if (currently_on_text) {
                    currently_on_text = false;
                    tree_mem_usage += sizeof('\0');
                    if (stacklen == 0) ++root_length;
                    else (*(size_t*)stack[stacklen - 1])++;
                }
                if (c == ')' || c == '\0') {
                    if (stacklen == 0) {
                        if (c == '\0') break;
                    } else {
                        if (stacklen != 0) {
                            tree_mem_usage += *(size_t*)stack[--stacklen] * sizeof(struct Node) + sizeof(struct Group);
                        }
                    }
                } else {
                    if (stacklen == 0) ++root_length;
                    else (*(size_t*)stack[stacklen - 1])++;
                    stack[stacklen++] = &overlay_lengths[cur_overlay++];
                }
                if (c == '(') {
                } else {
                    if (stacklen != 0) --stacklen;
                }
            } else if (c == '\\') {
                escaped = true;
            } else {
                tree_mem_usage += sizeof(c);
                currently_on_text = true;
            }
        }
    }

    /* Stage 3 */
    {
        if (maxstack != 0) free(overlay_lengths);
    }

    return SUCCESS;
}
