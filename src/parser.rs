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
    value: usize,
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

#[derive(Debug)]
pub struct ParsingResult {
    pub root: Group,
    pub unexpected_closers: Vec<Index>,
    pub unclosed_openers: Vec<Index>,
}

pub fn parse(input: &str) -> ParsingResult {
    let mut root_nodes = vec![];
    struct Overlay {
        idx: Index,
        group: Group,
    }
    let mut overlays: Vec<Overlay> = vec![];

    let mut idx = Index { value: 0 };

    let mut sbuf = String::new();
    let mut sbufidx = idx;

    let mut unclosed_openers = vec![];
    let mut unexpected_closers = vec![];

    let mut escaped = false;

    loop {
        let curidx = idx;
        let c = unsafe { input.get_unchecked(idx.value..) }.chars().next();
        if let Some(c) = c {
            idx.value += c.len_utf8();
            if escaped {
                sbuf.push(c);
                escaped = false;
                continue;
            }
        }

        if let None | Some('(' | ')') = c {
            let mut finished_layer = None;
            if let None | Some(')') = c {
                finished_layer = overlays.pop();
                if finished_layer.is_none() && c.is_some() {
                    unexpected_closers.push(curidx);
                }
            }
            let top = match overlays.last_mut() {
                Some(overlay) => &mut overlay.group.nodes,
                None => &mut root_nodes,
            };
            if !sbuf.is_empty() {
                top.push(Node {
                    kind: NodeKind::String(sbuf),
                    idx: sbufidx,
                });
                sbuf = String::new();
                sbufidx = idx;
            }
            if let Some(finished_layer) = finished_layer {
                let opener_idx = finished_layer.idx;
                top.push(Node {
                    idx: finished_layer.idx,
                    kind: NodeKind::Group(finished_layer.group),
                });
                if let None = c {
                    unclosed_openers.push(opener_idx);
                    continue;
                }
            }
            if let None = c {
                break;
            }
            if let Some('(') = c {
                overlays.push(Overlay {
                    idx: curidx,
                    group: Group { nodes: vec![] },
                })
            }
        } else if let Some(c) = c {
            if c == '\\' {
                escaped = true;
            } else {
                sbuf.push(c);
            }
        }
    }
    ParsingResult {
        unclosed_openers,
        unexpected_closers,
        root: Group { nodes: root_nodes },
    }
}
