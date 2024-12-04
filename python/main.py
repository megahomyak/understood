from types import SimpleNamespace as SN

def parse(inp):
    def parsell(unc, unx, inp):
        stack = [(0, [])]
        for i, c in enumerate(inp):
            if c == "(":
                new = (i, [])
                stack[-1][1].append(new)
                stack.append(new)
            elif c == ")":
                if len(stack) == 1:
                    unx.append(i)
                else:
                    stack.pop()
            else:
                stack[-1][1].append((i, c))
        while len(stack) != 1:
            unc.append(stack.pop()[0])
        return stack[0][1]

    def prettify(parsed_inp):
        sbuf = ""
        sbufidx = 0
        pretty = []
        for idx, layer in parsed_inp:
            if isinstance(layer, str):
                if not sbuf:
                    sbufidx = idx
                sbuf += layer
            else:
                if sbuf:
                    pretty.append((sbufidx, sbuf))
                    sbuf = ""
                pretty.append((idx, prettify(layer)))
        if sbuf:
            pretty.append((sbufidx, sbuf))
        return pretty

    def name(prettified):
        named = []
        for idx, layer in prettified:
            named.append(SN(idx=idx, value=layer if isinstance(layer, str) else name(layer)))
        return named
    unx = []
    unc = []
    res = name(prettify(parsell(unx, unc, inp)))
    return unx, unc, res

print(parse("""introduction (
    page (
        image ()
        description ()
    )a
)"""))
