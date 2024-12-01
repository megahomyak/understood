mod parser;
mod cli;
mod interpreter;

fn main() {
    loop {
        interpreter::execute(cli::read());
    }
}
