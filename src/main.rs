enum Part {
    Text(String),
    Group(Group),
}

type Index = usize;
struct Group {
    commands: Vec<(Part, Index)>,
}

struct Parser<'a> {
    idx: usize,
    s: &'a str,
    unexpected_closers: Vec<Index>,
    unclosed_openers: Vec<Index>,
}

impl<'a> Parser<'a> {
    fn next(&mut self) -> Option<char> {
        let c = unsafe { &self.s.get_unchecked(self.idx..) }.chars().next();
        if let Some(c) = c {
            self.idx += c.len_utf8();
        }
        c
    }
    fn parse_list(&mut self) -> Group {
        let mut root = Group { commands: vec![] };
        let mut str_idx = self.idx;
        let mut current_level = &mut root;
        let mut levels = Vec::new();
        let mut strbuf = String::new();
        loop {
            match self.next() {
                c @ (None | Some(')') | Some('(')) => {
                    if !strbuf.is_empty() {
                        current_level.commands.push((Part::Text(strbuf), str_idx));
                        strbuf = String::new();
                        str_idx = self.idx;
                    }
                    match c {
                        None => break,
                        Some('(') => levels.push(),
                        Some(')') => ,
                    }
                },
                Some(c) => {
                    strbuf.push(c);
                },
            }
        }
    }
    fn parse(&mut self) -> Group {
    
    }
}

fn main() {}
