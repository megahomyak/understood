enum Command {
    LoadName(String),
    Invocation(Program),
}

type Index = usize;
struct Program {
    commands: Vec<(Command, Index)>,
}

struct Iter<'a> {
    idx: usize,
    s: &'a str,
}
impl<'a> Iter<'a> {
    fn next(&mut self) -> Option<char> {
        let c = unsafe { &self.s.get_unchecked(self.idx..) }.chars().next();
        if let Some(c) = c {
            self.idx += c.len_utf8();
        }
        c
    }
}

fn parse(i: &mut Iter) -> Program {

}

fn main() {}
