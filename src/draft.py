root = []
stack = []

input_ = r"abc(d\(ef(\\g()h))i)blah((" + "\\"
idx = 0

sbuf = ""
sbufidx = 0

unclosed_openers = []
unexpected_closers = []

escaped = False

def nextiter(): # Just some Python convenience
    global idx
    try:
        c = input_[idx]
    except IndexError:
        return None
    else:
        idx += 1
        return c

while True:
    curidx = idx
    c = nextiter()
    if escaped and c is not None:
        sbuf += c
        escaped = False
        continue
    if c == "(" or c == ")" or c is None:
        try:
            top = stack[-1][1] # [1] = "take the actual group, not the idx" (which is also in the same tuple)
        except IndexError:
            top = root
        if sbuf != "":
            top.append((sbufidx, sbuf))
        if c is None:
            break
        sbuf = ""
        sbufidx = idx
        if c == "(":
            new = []
            # Alloc memory for (curidx, new) here
            top.append((curidx, new))
            stack.append((curidx, new))
        elif c == ")":
            try:
                stack.pop()
            except IndexError:
                unexpected_closers.append(curidx)
    else:
        if c == "\\":
            escaped = True
        else:
            sbuf += c
for groupidx, _group in stack:
    unclosed_openers.append(groupidx)
    # Dealloc memory for (curidx, new) here

print(f"input = {input_}")
print(f"{root = }")
print(f"{unclosed_openers = }")
print(f"{unexpected_closers = }")
