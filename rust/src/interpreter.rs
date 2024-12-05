pub struct Executor {}
impl Executor {
    pub fn new() -> Self {
        Self {}
    }
    pub fn execute(&mut self, program: crate::parser::Group) {
        println!("{:#?}", program);
    }
}
