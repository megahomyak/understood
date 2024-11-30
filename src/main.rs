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

struct Parsed {
    group: Group,
    unexpected_closers: Vec<Index>,
    unclosed_openers: Vec<Index>,
}

fn parse(s: &str) -> Parsed {
    let mut unexpected_closers = vec![];
    let mut unclosed_openers = vec![];
    enum GroupNode {
        Root {
            group: Group,
        },
        Overlay {
            group: Group,
            opener: Index,
            previous: *mut GroupNode,
        },
    }
    impl GroupNode {
        fn group(&mut self) -> &mut Group {
            match self {
                Self::Root { group } => group,
                Self::Overlay { group, .. } => group,
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
