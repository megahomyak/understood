use std::collections::HashMap;

use crate::parser::NodeKind;

pub struct Key {
    pub value: usize,
}

pub struct Object {
    associations: HashMap<Key, std::ffi::c_void>,
}
impl Object {
    pub fn new() -> Self {
        Self { associations: HashMap::new() }
    }
    pub fn set(&mut self, ) {

    }
}

type Name = String;
struct Scope {
    objects: HashMap<Name, Object>,
}

pub struct Executor {
    free_key: usize,
    root_scope: Scope,
    last_object: Object,
}
impl Executor {
    pub fn new() -> Self {
        Self {
            free_key: 0,
            last_object: Object
        }
    }
    pub fn execute(&mut self, program: crate::parser::Group) {
        for node in program.nodes {
            match node.kind {
                NodeKind::String(..) => (),
                NodeKind::Group(command) => for command in invocation {
                    match command.kind {
                        NodeKind::String(s) => 
                    }
                }
            }
        }
    }
}
