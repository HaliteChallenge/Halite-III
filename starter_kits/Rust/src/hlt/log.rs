use std::fs::File;
use std::io::Write;
use std::process::exit;
use std::sync::Mutex;
use std::time::SystemTime;
use std::time::UNIX_EPOCH;

lazy_static! {
    static ref LOG: Mutex<Log> = Mutex::new(Log::new());
}

pub struct Log {
    log_buffer: Option<Vec<String>>,
    file: Option<File>,
}

impl Log {
    pub fn new() -> Log {
        Log { log_buffer: Some(Vec::new()), file: None }
    }

    pub fn open(bot_id: usize) {
        let mut log = LOG.lock().unwrap();

        if log.file.is_some() {
            Log::panic_inner(&mut log, &format!("Error: log: tried to open({}) but we have already opened before.", bot_id));
        }

        let filename = format!("bot-{}.log", bot_id);
        let mut file = File::create(&filename).expect(&format!("Couldn't open file {} for logging!", &filename));

        Log::dump_log_buffer(&log.log_buffer, &mut file);

        log.file = Some(file);
        log.log_buffer = None;
    }

    pub fn log(message: &str) {
        let mut log = LOG.lock().unwrap();

        match &mut log.file {
            Some(file) => {
                writeln!(file, "{}", message).unwrap();
                return;
            },
            None => ()
        }

        match &mut log.log_buffer {
            Some(log_buffer) => {
                log_buffer.push(message.to_string());
            },
            None => panic!("Error: both file and log_buffer as missing.")
        }
    }

    pub fn flush() {
        let mut log = LOG.lock().unwrap();

        match &mut log.file {
            Some(file) => { file.flush().unwrap(); },
            None => (),
        }
    }

    pub fn panic(message: &str) -> ! {
        let mut log = LOG.lock().unwrap();
        Log::panic_inner(&mut log, message)
    }

    fn panic_inner(log: &mut Log, message: &str) -> ! {
        if log.file.is_none() {
            let timestamp = SystemTime::now().duration_since(UNIX_EPOCH).unwrap();
            let filename = format!("bot-unknown-{}.log", timestamp.as_secs());
            let file = File::create(&filename).expect(&format!("Couldn't open file {} for logging!", &filename));
            log.file = Some(file);
        }

        let file = match &mut log.file {
            Some(file) => file,
            None => panic!("Error: file should exist!")
        };

        Log::dump_log_buffer(&log.log_buffer, file);

        writeln!(file, "{}", message).unwrap();
        file.flush().unwrap();

        exit(1);
    }

    fn dump_log_buffer(log_buffer: &Option<Vec<String>>, file: &mut File) {
        match log_buffer {
            Some(log_buffer) => {
                for message in log_buffer {
                    writeln!(file, "{}", message).unwrap();
                }
            }
            None => ()
        }
    }
}
