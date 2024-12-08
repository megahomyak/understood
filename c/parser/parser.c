#include <stdlib.h>
#include <stdbool.h>

struct Node {
    enum {
        TEXT,
        GROUP,
    } type;
    union {
        char* text;
        struct Group* group;
    };
};

struct Group {
    size_t length;
    struct Node contents[];
};

struct _identified {
    size_t tree_mem;
    size_t max_overlays_len;
    size_t unclosed_openers_count;
    size_t unexpected_closers_count;
};

void _identify(char* input, struct _identified* output) {
    size_t tree_mem = 0;
    size_t idx = 0;
    size_t max_overlays_len = 0;
    size_t unclosed_openers_count = 0;
    size_t unexpected_closers_count = 0;
    bool currently_inside_text = false;
    for (;;) {
        char c = input[idx++];
        if (c == '\0' || c == '(' || c == ')') {
            if (currently_inside_text) {
                currently_inside_text = false;
                tree_mem += sizeof(char) + sizeof(struct Node);
            }
            if (c == '\0') break;
            if (c == '(') {
                tree_mem += sizeof(struct Group) + sizeof(struct Node);
                ++unclosed_openers_count;
                if (unclosed_openers_count > max_overlays_len) {
                    max_overlays_len = unclosed_openers_count;
                }
            }
            else {
                if (unclosed_openers_count == 0) ++unexpected_closers_count;
                else --unclosed_openers_count;
            }
        } else {
            tree_mem += sizeof(c);
            currently_inside_text = true;
        }
    }

    output->tree_mem = tree_mem;
    output->unclosed_openers_count = unclosed_openers_count;
    output->unexpected_closers_count = unexpected_closers_count;
    output->max_overlays_len = max_overlays_len;
}

void _
