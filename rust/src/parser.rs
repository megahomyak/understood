pub enum NodeKind {
    String(String),
    Group(Group),
}
impl std::fmt::Debug for NodeKind {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::String(s) => std::fmt::Debug::fmt(s, f),
            Self::Group(g) => std::fmt::Debug::fmt(g, f),
        }
    }
}

#[derive(Clone, Copy)]
pub struct Index {
    pub value: usize,
}
impl std::fmt::Debug for Index {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_str("Index { ")?;
        std::fmt::Debug::fmt(&self.value, f)?;
        f.write_str(" }")
    }
}

pub struct Node {
    pub idx: Index,
    pub kind: NodeKind,
}
impl std::fmt::Debug for Node {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_str("(")?;
        std::fmt::Debug::fmt(&self.idx, f)?;
        f.write_str(", ")?;
        std::fmt::Debug::fmt(&self.kind, f)?;
        f.write_str(")")
    }
}

pub struct Group {
    pub nodes: Vec<Node>,
}
impl std::fmt::Debug for Group {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        std::fmt::Debug::fmt(&self.nodes, f)
    }
}
impl Group {
    pub fn empty() -> Self {
        Self { nodes: Vec::new() }
    }
}

#[derive(Debug)]
pub struct ParsingResult {
    pub root: Group,
    pub unexpected_closers: Vec<Index>,
    pub unclosed_openers: Vec<Index>,
}

pub fn parse(input: &str) -> ParsingResult {
    let mut root = Group::empty();
    struct Overlay {
        idx: Index,
        group: Group,
    }
    let mut overlays: Vec<Overlay> = vec![];

    let mut unclosed_openers = vec![];
    let mut unexpected_closers = vec![];

    let mut idx = Index { value: 0 };
    let mut curidx = idx;

    let mut sbuf = String::new();
    let mut sbufidx = idx;

    let mut escaped = false;

    loop {
        let c = unsafe { input.get_unchecked(idx.value..) }.chars().next().unwrap_or('\0');
        if c != '\0' {
            curidx = idx;
            idx.value += c.len_utf8();
            if escaped {
                escaped = false;
                if !(c == '(' || c == ')' || c == '\\') {
                    sbuf.push('\\');
                }
                sbuf.push(c);
                continue;
            }
        }

        if c == ')' || c == '\0' || c == '(' {
            if !sbuf.is_empty() {
                let top = match overlays.last_mut() {
                    None => &mut root,
                    Some(top) => &mut top.group,
                };
                top.nodes.push(Node {
                    idx: sbufidx,
                    kind: NodeKind::String(sbuf),
                });
                sbufidx = idx;
                sbuf = String::new();
            }
            if c == ')' || c == '\0' {
                match overlays.pop() {
                    None => {
                        if c == '\0' {
                            break;
                        } else {
                            unexpected_closers.push(curidx);
                        }
                    },
                    Some(old_top) => {
                        let new_top = match overlays.last_mut() {
                            None => &mut root,
                            Some(new_top) => &mut new_top.group,
                        };
                        new_top.nodes.push(Node {
                            idx: old_top.idx,
                            kind: NodeKind::Group(old_top.group),
                        });
                        if c == '\0' {
                            unclosed_openers.push(old_top.idx);
                        }
                    }
                }
            } else {
                overlays.push(Overlay {
                    idx: curidx,
                    group: Group::empty(),
                });
            }
        } else if c == '\\' {
            escaped = true;
        } else {
            sbuf.push(c);
        }
    }

    ParsingResult {
        unclosed_openers,
        unexpected_closers,
        root,
    }
}
