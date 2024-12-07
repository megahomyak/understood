#include <stdlib.h>
#include <stdbool.h>

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
    {
        size_t cur_overlay;
        if (maxstack != 0) {
            overlay_lengths = malloc(overlays_amount * sizeof(size_t) + maxstack * sizeof(size_t*));
            if (overlay_lengths == NULL) return FAILURE;
            memset(overlay_lengths, 0, overlays_amount * sizeof(size_t));
            cur_overlay = 0;
            stack = (size_t**) &overlay_lengths[overlays_amount];
        }
        size_t idx = 0;
        size_t escaped = false;
        size_t stacklen = 0;
        bool currently_on_text = false;
        for (;;) {
            char c = input[idx++];
            if (c != '\0' && escaped) {
                currently_on_text = true;
                escaped = false;
                continue;
            }
            if (c == '\0' || c == '(' || c == ')') {
                if (currently_on_text) {
                    currently_on_text = false;
                    if (stacklen == 0) ++root_length;
                    else (*stack[stacklen - 1])++;
                }
                if (c == '\0') break;
                if (c == '(') {
                    if (stacklen == 0) ++root_length;
                    else (*stack[stacklen - 1])++;
                    stack[stacklen++] = &overlay_lengths[cur_overlay++];
                } else {
                    if (stacklen != 0) --stacklen;
                }
            } else if (c == '\\') {
                escaped = true;
            } else {
                currently_on_text = true;
            }
        }
    }

    /* Stage 3 */

    // Do shit here

    if (maxstack != 0) free(overlay_lengths);
    return SUCCESS;
}
