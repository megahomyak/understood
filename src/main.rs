mod parser;

fn main() {
    println!("{:?}", parser::parse(r"abc(d\(ef(\\g()h))i)blah(("));
}
