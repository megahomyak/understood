from types import SimpleNamespace as SN

class Executor:
    def __init__(self):
        self.scope = {}
        self.free_key = 0
        self.last_object = create_nothing(display_key)
    def execute(program):
        for invocation in program:
            if isinstance(invocation, list):
                for command in invocation:
                    if isinstance(command, str):
                        self.last_object = self.scope[command]
                    if isinstance(command, list):
                        self.last_object(input_=command, context=SN(
                            scope=self.scope,
                            make_new_key=self.make_new_key, # TODO: do not pass this in object invocations, only pass this in plugin initialisations
                        ))

def parse(input_):
    root = []
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
                if not (c == "(" or c == ")" or c == "\\"):
                    sbuf += "\\"
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
                top = overlays[-1].group
            except IndexError:
                top = root
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
                overlays.append(SN(idx=curidx, group=[]))
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
    executor = Executor()
    while True:
        program = prompt_for_program()
        executor.execute(program)

main()
