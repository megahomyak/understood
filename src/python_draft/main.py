from types import SimpleNamespace as SN

class Object:
    def __init__(self):
        self.associations = {}
    def set(self, k, v):
        # NOTE: in prod code, make setting the pointer (second arg, "value") to NULL remove the entry
        self.associations[k] = v
    def get(self, k):
        # NOTE: in prod code, if the entry is not found, return null pointer (because a pointer will be returned anyway, so it makes sense to do this little unification)
        return self.associations[k]

class KeyStorage:
    def __init__(self):
        self.free_key = 0
    def make_new_key(self):
        new_key = self.free_key
        self.free_key += 1
        return new_key

class Executor:
    def __init__(self):
        self.scope = {}
        self.free_key = 0
        self.last_object = create_nothing(display_key)
    def execute(program):
        for invocation in program.nodes:
            if isinstance(invocation, list):
                for command in invocation.nodes:
                    if isinstance(command, str):
                        self.last_object = self.scope[command]
                    if isinstance(command, list):
                        self.last_object(input_=command, context=SN(
                            scope=self.scope,
                        ))

def parse(input_):
    root = SN(nodes=[])
    overlays = []

    idx = 0

    sbuf = ""
    sbufidx = 0

    unclosed_openers = []
    unexpected_closers = []

    escaped = False

    while True:
        curidx = idx
        try:
            c = input_[idx]
        except IndexError:
            c = None
        else:
            idx += 1
            if escaped:
                sbuf += c
                escaped = False
                continue

        if c is None or c == "(" or c == ")":
            finished_layer = None
            if c is None or c == ")":
                try:
                    finished_layer = overlays.pop()
                except IndexError:
                    if c is not None:
                        unexpected_closers.append(curidx)
            try:
                top = overlays[-1].group.nodes
            except IndexError:
                top = root.nodes
            if sbuf != "":
                top.append(SN(value=sbuf, idx=sbufidx))
                sbuf = ""
                sbufidx = idx
            if finished_layer is not None:
                opener_idx = finished_layer.idx
                top.append(SN(idx=finished_layer.idx, value=finished_layer.group))
                if c is None:
                    unclosed_openers.append(opener_idx)
                    continue
            if c is None:
                break
            if c == "(":
                overlays.append(SN(idx=curidx, group=SN(nodes=[])))
        else:
            if c == "\\":
                escaped = True
            else:
                sbuf += c
    return SN(
        unclosed_openers=unclosed_openers,
        unexpected_closers=unexpected_closers,
        root=root,
    )

def prompt_for_program():
    progbuf = ""
    is_appending = False
    while True:
        prompt = ("+" if is_appending else ">") + " "
        progbuf += input(prompt) + "\n"
        res = parse(progbuf)
        if not res.unclosed_openers:
            if res.unexpected_closers:
                print(f"! Unexpected closers: {res.unexpected_closers}")
            print("===")
            return res.root
        else:
            is_appending = True

def main():
    program = prompt_for_program()
    executor = Executor()
    executor.execute(program)

main()
