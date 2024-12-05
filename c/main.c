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
Memory layout:
[unclosed_openers]
[unexpected_closers]
- just simple arrays, only need the length and it's easily retrievable
root+root length: a group of the specified length (an array of Node) (=Group Node, just not in-place)
Node is: {type, idx, ptr}
Type=text: {first char, rest of chars until '\0'}
Type=group: {len (size_t), [Node]}

computation of lengths of text and groups (that is, simply indexes: the post-end index for text and the post-end for groups or something similar) using an array where every next item is a new Node; while computing that, use a stack where store pointers to groups where you descend. I can probably potentially just store pointers to beginnings as well, just so that I won't even have to scan anything later

there is a way to get the length of every item just by looking at its neighbors (and their neighbors, and so on - recursively and going to the very end of every group) and the length of the overall memory allocated for the parsing result (given that it's exactly the right amount), but that's gonna be costly asf, so I'm not doing it here
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
        } else if (c == '\\') {
            escaped = true;
        } else {
            ++sbufsize;
        }
        ++idx;
    }
    /* Stage 2 */
    idx = 0;
    escaped = false;
    void* overlays = malloc(max_overlay_level * sizeof(size_t*) + nodes_amount * sizeof(size_t));
    if (overlays == NULL) {
        return FAILURE;
    }
    void* node_lengths = overlays + max_overlay_level * sizeof(size_t*);
    for (;;) {
        c = input[idx];
        if (c == '\0') break;
    }
    free(overlays);
    /* Stage 3 */
    return SUCCESS;
}

void free_parsing_result(struct ParsingResult* r) {
    free(r->unclosed_openers);
}
