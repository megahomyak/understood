enum Node {
    String(String),
    Group(Group),
}
impl std::fmt::Debug for Node {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::String(s) => std::fmt::Debug::fmt(s, f),
            Self::Group(g) => std::fmt::Debug::fmt(g, f),
        }
    }
}

type Index = usize;
struct Group {
    nodes: Vec<(Index, Node)>,
}
impl std::fmt::Debug for Group {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        std::fmt::Debug::fmt(&self.nodes, f)
    }
}

struct ParsingResult {
    root: Group,
    unexpected_closers: Vec<Index>,
    unclosed_openers: Vec<Index>,
}

fn parse(input: &str) -> ParsingResult {
    let mut root = vec![];
    struct Overlay {
        idx: Index,
        group: Group,
    }
    let mut overlays = vec![];

    let mut idx = 0;

    let mut sbuf = String::new();
    let mut sbufidx = 0;

    let mut unclosed_openers = vec![];
    let mut unexpected_closers = vec![];

    let mut escaped = false;

    loop {
        let curidx = idx;
        let c = unsafe { input.get_unchecked(idx..) }.chars().next();
        if let Some(c) = c {
            idx += c.len_utf8();
            if escaped {
                sbuf.push(c);
                escaped = false;
                continue;
            }
        }
        match c {
            Some('(') | Some(')') | None => {
                let mut top = overlays.last_mut().map(|overlay| &mut overlay.group).unwrap_or(&mut root);
                if !sbuf.is_empty() {
                    top.push((sbufidx, Node::String(sbuf)));
                }
                match c {
                    None => break,
                    _ => (),
                }
                sbuf = String::new();
                sbufidx = idx;
                match c {
                    Some('(') => {
                        let mut new = Box::into_raw(Box::new(Overlay {
                            idx: 
                        }));
                        let mut new = vec![];
                    },
                    Some(')') => {},
                    _ => unreachable!(),
                }
            },
            Some(c) =>
                if c == '\\' {
                    escaped = true;
                } else {
                    sbuf.push(c);
                }
        }
    }

    let mut curgroup = Box::into_raw(Box::new(GroupNode::Root {
        group: Group { nodes: vec![] },
    }));
    let mut idx = 0;
    let mut escaped = false;
    let mut textbuf = String::new();
    let mut textidx = 0;
    loop {
        let c = unsafe { s.get_unchecked(idx..) }.chars().next();
        let charidx = idx;
        if let Some(c) = c {
            idx += c.len_utf8();
            if escaped {
                textbuf.push(c);
                escaped = false;
                continue;
            }
        }
        match c {
            None | Some(')') | Some('(') => {
                if !textbuf.is_empty() {
                    unsafe { curgroup.read() }
                        .group()
                        .nodes
                        .push((Node::Text(textbuf), textidx));
                    textbuf = String::new();
                    textidx = idx;
                }
                match c {
                    None => break,
                    Some(')') => match unsafe { curgroup.read() } {
                        GroupNode::Root { .. } => unexpected_closers.push(charidx),
                        GroupNode::Overlay { group, opener, previous } => {
                            *unsafe { previous.read() }.group() = group;
                        }
                    }
                }
            }
            Some(c) => {
                if c == '\\' {
                    escaped = true;
                } else {
                    textbuf.push(c);
                }
            }
        }
    }
    loop {
        match unsafe { curgroup.read() } {
            GroupNode::Overlay { group, opener, previous } => {
                let mut overlay = curgroup;
            }
        }
    }
    Parsed {
        group: match unsafe { curgroup.read() } {

        }
    }
}

fn main() {
}
