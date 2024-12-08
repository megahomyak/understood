#include "parser.c"
#include <stdio.h>

#define show_me(format, expr) printf("%s = " format "\n", #expr, expr)
#define check_tree_mem(s, expectation) {struct _identified result; _identify(s, &result); printf("%s - " #s " - expected %zu, got %zu\n", result.tree_mem == expectation ? "SUCCESS" : "FAILURE", expectation, result.tree_mem);}

void main(void) {
    show_me("%zu", sizeof(struct Node));
    show_me("%zu", sizeof(struct Group));
    show_me("%zu", sizeof(size_t));
    check_tree_mem("", (size_t)0);
    check_tree_mem("()",
        + sizeof(struct Node)
        + sizeof(struct Group)
    );
    check_tree_mem("ab",
        + sizeof(struct Node)
        + sizeof(char)*3
    );
    check_tree_mem("abc",
        + sizeof(struct Node)
        + sizeof(char)*4
    );
    check_tree_mem("ab)c",
        + sizeof(struct Node)
        + sizeof(char)*3
        + sizeof(struct Node)
        + sizeof(char)*2
    );
    check_tree_mem("abc (",
        + sizeof(struct Node)
        + sizeof(char)*5
        + sizeof(struct Node)
        + sizeof(struct Group)
    );
    check_tree_mem("abc ()",
        + sizeof(struct Node)
        + sizeof(char)*5
        + sizeof(struct Node)
        + sizeof(struct Group)
    );
    check_tree_mem("abc (blah())a",
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
