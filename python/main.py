from types import SimpleNamespace as SN

def und_get_top(overlays, root):
    try: return overlays[-1]
    except: return root

def parse_und(s):
    root = []
    overlays = []
    unclosed_openers = []
    unexpected_closers = []
    nextidx = 0
    curidx = nextidx
    textbuf = ""
    textidx = nextidx
    escaped = False
    while True:
        try: c = s[nextidx]
        except: c = None
        if c is not None:
            curidx = nextidx
            nextidx += 1
        if escaped:
            escaped = False
            if not (c == "(" or c == ")"):
                textbuf += "\\"
            if c is not None:
                textbuf += c
            continue
        if c is None or c == "(" or c == ")":
            if textbuf != "":
                top = und_get_top(overlays, root)
                top.append(SN(type="text", idx=textidx, value=textbuf))
                textbuf = ""
            textidx = nextidx
            if c == ")" or c is None:
                try:
                    old_top = overlays.pop()
                except:
                    if c is None:
                        break
                    else:
                        unexpected_closers.append(curidx)
                else:
                    new_top = und_get_top(overlays, root)
                    new_top.append(type="group", idx=old_top.idx, value=old_top.group)
                    if c is None:
                        unclosed_openers.append(old_top.idx)
            else:
                overlays.append(SN(group=[], idx=curidx))
        elif c == "\\":
            escaped = True
        else:
            textbuf += c
    return SN(unexpected_closers=unexpected_closers, unclosed_openers=unclosed_openers, root=root)

def und_to_pon(und):
    program = []
    for und_node in und:
        if und_node.type == "group":
            program.append(SN(idx=und_node.idx, type="invocation", value=und_node.value))
        elif und_node.type == "text":
            words = []
            nextidx = 0
            wordbuf = ""
            nameidx = nextidx
            escaped = False
            while True:
                try: c = und_node.value[nextidx]
                except: c = None
                if c is not None:
                    nextidx += 1
                if escaped:
                    escaped = False
                    if c is None or not (c == "\\" or c.isspace()):
                        wordbuf += "\\"
                    if c is not None:
                        wordbuf += c
                    continue
                if c is None:
                    is_boundary = True
                else:
                    if c.isspace() and words == []:
                        nameidx = nextidx
                    is_boundary = c == ")" or c == "(" or c.isspace()
                    if not is_boundary:
                        if c == "\\":
                            escaped = True
                        else:
                            wordbuf += c
                    if is_boundary:
                        if wordbuf != "":
                            words.append(wordbuf)
                            wordbuf = ""
                        if c is None:
                            break
            if words != []:
                program.append(SN(idx=nameidx + und_node.idx, type="name", value=words))

def execute(pon):
    # `init(scope, propgetter: function, propsetter: function) -> None` OR maybe `init(scope, input: str | None, propgetter: function, propsetter: function) -> None`
    # init(getkey) -> obj # A "key" is a pointer or something similar. This is to avoid dealing with strings all the time, helps prevent abuse of them. Although not as required and can probably be the following
    # init(get, set) -> obj
    # fn(scope, input) -> obj
    # `func(scope, input: str) -> None`
    # Scope: (varname, varvalue, next) | None
    #
    # import (libname) ()
    # vs
    # import (libname)()
    # vs
    # import (libname)
    # The last one seems to be the cleanest
    #
    # import (libname(params)) <- library names will have to consist of words in this case as well, which I don't want
    #
    # Btw, I can just wrap stuff in standard library functions (that will be in prelude):
    # LIBRARY INTERFACE:
    # init(get, set) -> (&ctx, obj)
    # fn(&ctx, scope, input) -> obj
    #
    # USER INTERFACE:
    # `import (libname)` # Calls "init", then "fn" with "()" as input
    # `raw import (libname) (params)` # Calls "init", then "fn" with "(params)" as input
    # Basically, `import (a)`=`raw import (a) ()`
    pass

def main():
    pass

main()
