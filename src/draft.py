root = []
stack = []
sbuf = ""
iter_ = iter("abc(def(g()h))i")
def nextiter():
    try:
        return next(iter_)
    except StopIteration:
        return None
while True:
    c = nextiter()
    if c == "(" or c == ")" or c is None:
        try:
            top = stack[-1]
        except IndexError:
            top = root
        if sbuf != "":
            top.append(sbuf)
        if c is None:
            break
        sbuf = ""
        if c == "(":
            new = []
            top.append(new)
            stack.append(new)
        elif c == ")":
            try:
                stack.pop()
            except IndexError:
                pass # Unexpected closer
    else:
        sbuf += c
for node in stack:
    pass # Unclosed opener

assert root == ["abc", ["def", ["g", [], "h"]], "i"]
