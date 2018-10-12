use std::fs::File;
use std::io::Write;
use std::process::exit;
use std::time::SystemTime;
use std::time::UNIX_EPOCH;

pub struct Log {
    log_buffer: Option<Vec<String>>,
    file: Option<File>,
}

impl Log {
    pub fn new() -> Log {
        Log { log_buffer: Some(Vec::new()), file: None }
    }

    pub fn open(&mut self, bot_id: usize) {
        if self.file.is_some() {
            self.panic(&format!("Error: log: tried to open({}) but we have already opened before.", bot_id));
        }

        let filename = format!("bot-{}.log", bot_id);
        let file = File::create(&filename).expect(&format!("Couldn't open file {} for logging!", &filename));
        self.file = Some(file);

        self.dump_log_buffer();

        self.log_buffer = None;
    }

    pub fn log(&mut self, message: &str) {
        match &mut self.file {
            Some(file) => {
                writeln!(file, "{}", message).unwrap();
                return;
            },
            None => ()
        }

        match &mut self.log_buffer {
            Some(log_buffer) => {
                log_buffer.push(message.to_string());
            },
            None => panic!("Error: both file and log_buffer as missing.")
        }
    }

    pub fn flush(&mut self) {
        match &mut self.file {
            Some(file) => { file.flush().unwrap(); },
            None => (),
        }
    }

    pub fn panic(&mut self, message: &str) -> ! {
        if self.file.is_none() {
            let timestamp = SystemTime::now().duration_since(UNIX_EPOCH).unwrap();
            let filename = format!("bot-unknown-{}.log", timestamp.as_secs());
            let file = File::create(&filename).expect(&format!("Couldn't open file {} for logging!", &filename));
            self.file = Some(file);
        }

        self.dump_log_buffer();

        let file = match &mut self.file {
            Some(file) => file,
            None => panic!("Error: file should exist!")
        };

        writeln!(file, "{}", message).unwrap();
        file.flush().unwrap();

        exit(1);
    }

    fn dump_log_buffer(&mut self) {
        let file = match &mut self.file {
            Some(file) => file,
            None => panic!("Error: file should exist!")
        };

        match &self.log_buffer {
            Some(log_buffer) => {
                for message in log_buffer {
                    writeln!(file, "{}", message).unwrap();
                }
            }
            None => ()
        }
    }
}
