#include <stdio.h>

#define arrsize(arr) (sizeof(arr) / sizeof(arr[0]))

struct ctx {
    char* prog;
    char* vars[25];
};

void blah(struct ctx* ctx) {
    printf("blah\n");
}

struct {
    char* name;
    void (*handler)(struct ctx*);
} functions[] = {
    { .name = "blah", .handler = blah },
};

int namecmp(struct ctx* ctx, char* pattern) {
    /* Assumes the `prog` can't have a name without a following opening paren */
    while (*ctx->prog == ' ' || *ctx->prog == '\n' || *ctx->prog == '\t') ++ctx->prog;
    if (*ctx->prog == '\0') return 0;
    for(;;) {
        if (*pattern == '\0') {
            if (*ctx->prog == '(') return 1;
            else return 0;
        }
        if (*pattern != *ctx->prog) return 0;
        ++ctx->prog;
    }
}

void execute_program(struct ctx* ctx) {
    int nesting = 0;
    for(;;) {
        if (namecmp(ctx, "test1")) {

        }
    }
}

int main(void) {
    char* prog = "test1( test2() test3() )";
    struct ctx ctx = { .var1 = 123, .prog = prog };
    execute_program(&ctx);
}
