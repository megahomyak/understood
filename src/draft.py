root = []
overlays = []

input_ = r"abc(d\(ef(\\g()h))i)blah(("
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
        newgroup = None
        if c == ")" or c is None:
            try:
                newgroup = overlays.pop()
            except IndexError:
                if c is not None:
                    unexpected_closers.append(curidx)
        try:
            top = overlays[-1][1] # [1] = group
        except IndexError:
            top = root
        if sbuf != "":
            top.append((sbufidx, sbuf))
            sbuf = ""
            sbufidx = idx
        if newgroup is not None:
            groupidx = newgroup[0] # [0] = idx
            top.append(newgroup)
            if c is None:
                unclosed_openers.append(groupidx)
                continue
        if c is None:
            break
        if c == "(":
            overlays.append((curidx, []))
    else:
        if c == "\\":
            escaped = True
        else:
            sbuf += c

print(f"input = {input_}")
print(f"{root = }")
print(f"{unclosed_openers = }")
print(f"{unexpected_closers = }")
