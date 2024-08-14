use crate::console::Console;
use voyageurs::listener::KeyboardHandler;

pub struct Terminal {
    console: Console,
}

impl KeyboardHandler for Terminal {
    fn handle_char(&mut self, c: char) {
        self.console.write_char(c)
    }
}

impl Terminal {
    pub fn new(console: Console) -> Self {
        Self { console }
    }
}
