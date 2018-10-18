use hlt::log::Log;
use std::io::stdin;
use std::process::exit;
use std::str::FromStr;

pub struct Input {
    tokens: Vec<String>,
    current_token: usize,
}

impl Input {
    pub fn new() -> Input {
        Input { tokens: Vec::new(), current_token: 0 }
    }

    pub fn read_and_return_line(&mut self) -> String {
        let mut buf = String::new();
        match stdin().read_line(&mut buf) {
            Ok(_) => (),
            Err(_) => {
                Log::log("Input connection from server closed. Exiting...");
                Log::flush();
                exit(0);
            },
        }
        buf
    }

    pub fn read_and_parse_line(&mut self) {
        let buf = self.read_and_return_line();
        let token_iter = buf.split_whitespace();
        let token_iter = token_iter.filter(|x| !x.is_empty());
        let token_iter = token_iter.map(|x| x.to_string());
        self.tokens = token_iter.collect();
        self.current_token = 0;
    }

    pub fn next_i32(&mut self) -> i32 {
        self.next()
    }

    pub fn next_usize(&mut self) -> usize {
        self.next()
    }

    pub fn next<T: FromStr>(&mut self) -> T {
        let token = &self.tokens[self.current_token];
        self.current_token += 1;

        let result = match token.parse() {
            Ok(x) => x,
            Err(_) => Log::panic(&format!("Can't parse '{}'.", token)),
        };
        result
    }
}
