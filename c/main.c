#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

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

#define showme(format, expr) printf("%s = " format "\n", #expr, expr)

size_t parse(char* input) {
    size_t tree_mem = 0;
    size_t idx = 0;
    size_t max_overlays_len = 0;
    size_t overlays_len = 0;
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
                ++overlays_len;
                if (overlays_len > max_overlays_len) {
                    max_overlays_len = overlays_len;
                }
            }
            else {
                if (overlays_len == 0) ++unexpected_closers_count;
                else --overlays_len;
            }
        } else {
            tree_mem += sizeof(c);
            currently_inside_text = true;
        }
    }
    size_t unclosed_openers_count = overlays_len;
    overlays_len = 0;

    return tree_mem;
}

#define parsecheck(s, expectation) {size_t result = parse(s); printf("%s - " #s " - expected %zu, got %zu\n", result == expectation ? "SUCCESS" : "FAILURE", expectation, result);}

void main(void) {
    showme("%zu", sizeof(struct Node));
    showme("%zu", sizeof(struct Group));
    showme("%zu", sizeof(size_t));
    parsecheck("", (size_t)0);
    parsecheck("()",
        + sizeof(struct Node)
        + sizeof(struct Group)
    );
    parsecheck("ab",
        + sizeof(struct Node)
        + sizeof(char)*3
    );
    parsecheck("abc",
        + sizeof(struct Node)
        + sizeof(char)*4
    );
    parsecheck("ab)c",
        + sizeof(struct Node)
        + sizeof(char)*3
        + sizeof(struct Node)
        + sizeof(char)*2
    );
    parsecheck("abc (",
        + sizeof(struct Node)
        + sizeof(char)*5
        + sizeof(struct Node)
        + sizeof(struct Group)
    );
    parsecheck("abc ()",
        + sizeof(struct Node)
        + sizeof(char)*5
        + sizeof(struct Node)
        + sizeof(struct Group)
    );
    parsecheck("abc (blah())a",
        + sizeof(struct Node)
        + sizeof(char)*5
        + sizeof(struct Node)
        + sizeof(struct Group)
        + sizeof(struct Node)
        + sizeof(char)*5
        + sizeof(struct Node)
        + sizeof(struct Group)
        + sizeof(struct Node)
        + sizeof(char)*2
    );
}
