use core::str::Split;

use crate::console::Console;
use alloc::{
    format,
    string::{String, ToString},
};
use voyageurs::listener::KeyboardHandler;

pub struct Terminal {
    console: Console,
    curr_cmd: String,
    cwd: String,
    row: u32,
}

impl KeyboardHandler for Terminal {
    fn handle_char(&mut self, c: char) {
        let mut should_execute = false;
        match c {
            '\x08' => {
                let mut chars = self.curr_cmd.chars();
                chars.next_back(); // Pop last char.
                self.curr_cmd = chars.collect();
            }
            '\n' => should_execute = true,
            _ => self.curr_cmd.push(c),
        }
        self.rewrite_command();
        if should_execute {
            self.execute_command();
            self.rewrite_command();
        }
    }
}

impl Terminal {
    pub fn new(console: Console) -> Self {
        let mut term = Self {
            console,
            curr_cmd: String::new(),
            cwd: "/".to_string(),
            row: 0,
        };
        term.rewrite_command();
        term
    }

    fn rewrite_command(&mut self) {
        self.console.write_char('>', self.row, 0);

        let mut col = 1;
        for c in self.curr_cmd.chars() {
            self.console.write_char(c, self.row, col);
            col += 1;
        }

        // Hacky way to properly backspace.
        // FIXME: This won't work once we have line wrapping.
        self.console.write_char(' ', self.row, col)
    }

    fn write_line(&mut self, line: &str) {
        let mut col = 0;
        for c in line.chars() {
            self.console.write_char(c, self.row, col);
            col += 1;
        }

        self.row += 1
    }

    fn execute_command(&mut self) {
        self.row += 1;

        let curr_cmd = self.curr_cmd.clone();
        let mut tokens = curr_cmd.split(' ');

        match tokens.next() {
            None => {}
            Some(command) => self.execute_command_parsed(command, tokens),
        }

        self.curr_cmd.clear()
    }

    fn execute_command_parsed(&mut self, cmd: &str, _args: Split<'_, char>) {
        match cmd {
            "help" => self.write_line("Available commands are 'pwd', 'ls', 'cd', and 'exec'"),
            "pwd" => self.write_line(&self.cwd.clone()),
            _ => self.write_line(&format!("Unrecognized command: {}", cmd)),
        }
    }
}
