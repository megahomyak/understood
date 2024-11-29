enum Node {
    Text(String),
    Group(Group),
}
impl std::fmt::Debug for Node {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::Text(t) => std::fmt::Debug::fmt(t, f),
            Self::Group(g) => std::fmt::Debug::fmt(g, f),
        }
    }
}

type Index = usize;
struct Group {
    nodes: Vec<(Node, Index)>,
}
impl std::fmt::Debug for Group {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        std::fmt::Debug::fmt(&self.nodes, f)
    }
}
enum AfterGroup {
    End,
    Closer(Index),
}

#[derive(Debug)]
struct Parser<'a> {
    idx: usize,
    s: &'a str,
    unexpected_closers: Vec<Index>,
    unclosed_openers: Vec<Index>,
    textbuf: String,
    textidx: Index,
}

impl<'a> Parser<'a> {
    fn next(&mut self) -> Option<char> {
        let c = unsafe { &self.s.get_unchecked(self.idx..) }.chars().next();
        if let Some(c) = c {
            self.idx += c.len_utf8();
        }
        c
    }
    fn parse_group(&mut self) -> (Group, AfterGroup) {
        let mut nodes = vec![];
        loop {
            let charidx = self.idx;
            match self.next() {
                c @ (Some('(') | Some(')') | None) => {
                    if !self.textbuf.is_empty() {
                        nodes.push((Node::Text(std::mem::replace(&mut self.textbuf, String::new())), self.textidx));
                    }
                    self.textidx = self.idx;
                    match c {
                        None | Some(')') => {
                            return (
                                Group { nodes },
                                match c {
                                    None => AfterGroup::End,
                                    Some(')') => AfterGroup::Closer(charidx),
                                    _ => unreachable!(),
                                },
                            )
                        }
                        Some('(') => {
                            let (group, after) = self.parse_group();
                            nodes.push((Node::Group(group), charidx));
                            match after {
                                AfterGroup::Closer(_idx) => (),
                                AfterGroup::End => self.unclosed_openers.push(charidx),
                            }
                        }
                        _ => unreachable!(),
                    }
                }
                Some(c) => self.textbuf.push(c),
            }
        }
    }
    fn parse(&mut self) -> Group {
        let mut root_nodes = vec![];
        loop {
            let (group, after) = self.parse_group();
            root_nodes.extend(group.nodes);
            match after {
                AfterGroup::End => break,
                AfterGroup::Closer(idx) => self.unexpected_closers.push(idx),
            }
        }
        Group { nodes: root_nodes }
    }
}

fn main() {
    let mut parser = Parser {
        unexpected_closers: vec![],
        unclosed_openers: vec![],
        idx: 0,
        s: "a))b)c(((def",
        textidx: 0,
        textbuf: String::new(),
    };
    println!("{:?}", parser.parse());
    println!("{:?}", parser);
}
