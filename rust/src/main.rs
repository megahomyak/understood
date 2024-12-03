mod parser;
mod cli;
mod interpreter;

fn main() {
    let mut executor = interpreter::Executor::new();
    loop {
        executor.execute(cli::read());
    }
}
