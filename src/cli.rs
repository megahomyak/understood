use std::io::Write;

pub fn read() -> crate::parser::Group {
    let stdin = std::io::stdin();
    let mut stdout = std::io::stdout();
    let mut buf = String::new();
    let mut is_appending = false;
    loop {
        print!("{} ", if is_appending { "+" } else { ">" });
        stdout.flush().unwrap();
        if let Ok(0) = stdin.read_line(&mut buf) {
            panic!("Non-interactive use is not allowed yet");
        }
        let res = crate::parser::parse(&buf);
        if res.unclosed_openers.is_empty() {
            if !res.unexpected_closers.is_empty() {
                println!("! Unexpected closers: {:?}", res.unexpected_closers);
            }
            println!("===");
            break res.root;
        } else {
            is_appending = true;
        }
    }
}
