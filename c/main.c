#include <stdlib.h>
#include <stdbool.h>

struct ParsingResult {
    void* unclosed_openers;
    void* unexpected_closers;
    size_t root_length;
    void* root;
};

enum Result {
    SUCCESS,
    FAILURE,
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

STAGE 3:
REQUIREMENTS:
* How long each group is (separately: root node, overlays)
OUTPUT:
* The finished tree (with text lengths gathered directly from the input string)
*/
enum Result parse(struct ParsingResult* parsing_result, char* input) {
    /* Stage 1 */
    size_t unclosed_openers_count = 0;
    size_t max_overlay_level = unclosed_openers_count;
    size_t idx = 0;
    size_t unexpected_closers_count = 0;
    size_t nodes_amount = 0;
    size_t sbufsize = 0;
    bool escaped = false;
    char c;
    for (;;) {
        c = input[idx];
        if (escaped && c != '\0') {
            escaped = false;
            if (!(c == '(' || c == ')' || c == '\\')) {
                ++sbufsize;
            }
            ++sbufsize;
            continue;
        }
        if (c == '\0' || c == '(' || c == ')') {
            if (sbufsize != 0) {
                ++nodes_amount;
                sbufsize = 0;
            }
            if (c == '\0') break;
            if (c == '(') {
                ++nodes_amount;
                ++unclosed_openers_count;
                if (unclosed_openers_count > max_overlay_level) {
                    max_overlay_level = unclosed_openers_count;
                }
            } else {
                if (unclosed_openers_count == 0) {
                    ++unexpected_closers_count;
                } else {
                    --unclosed_openers_count;
                }
            }
            continue;
        } else if (c == '\\') escaped = true;
        else ++sbufsize;
        ++idx;
    }
    /* Stage 2 */
    size_t overlays_size = max_overlay_level * sizeof(size_t*) + nodes_amount * sizeof(size_t);
    void* overlays;
    void* node_lengths;
    if (overlays_size != 0) {
        overlays = malloc(overlays_size);
        if (overlays == NULL) return FAILURE;
        node_lengths = overlays + max_overlay_level * sizeof(size_t*);
    }
    size_t root_length = 0;
    idx = 0;
    escaped = false;
    unclosed_openers_count = 0;
    for (;;) {
        c = input[idx];
        if (escaped && c != '\0') {
            escaped = false;
            if (!(c == '(' || c == ')' || c == '\\')) {
                ++sbufsize;
            }
            ++sbufsize;
            continue;
        }
        if (c == '\0' || c == '(' || c == ')') {
            if (sbufsize != 0) {
                ++nodes_amount;
                sbufsize = 0;
            }
            if (c == '\0') break;
            if (c == '(') {
                ++nodes_amount;
                ++unclosed_openers_count;
                if (unclosed_openers_count > max_overlay_level) {
                    max_overlay_level = unclosed_openers_count;
                }
            } else {
                if (unclosed_openers_count == 0) {
                    ++unexpected_closers_count;
                } else {
                    --unclosed_openers_count;
                }
            }
            continue;
        } else if (c == '\\') escaped = true;
        else ++sbufsize;
        ++idx;
    }
    if (overlays_size != 0) {
        free(overlays);
    }
    /* Stage 3 */
    return SUCCESS;
}

void free_parsing_result(struct ParsingResult* r) {
    free(r->unclosed_openers);
}
