use alloc::string::String;
use alloc::vec::Vec;
use core::fmt;

pub struct Writer {
    int_vec: Vec<u8>,
}

impl Writer {
    pub fn new() -> Self {
        Self {
            int_vec: Vec::new(),
        }
    }
}

impl Into<String> for Writer {
    fn into(self) -> String {
        String::from_utf8(self.int_vec).expect("Failed to convert")
    }
}

impl fmt::Write for Writer {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        for c in s.bytes() {
            self.int_vec.push(c);
        }
        fmt::Result::Ok(())
    }
}

#[macro_export]
macro_rules! debug {
    () => {
        $crate::syscall::debug("");
    };
    ($fmt:literal) => {
        $crate::syscall::debug($fmt);
    };
    ($fmt:literal, $($val:expr),+) => {{
        use core::fmt::Write as _;
        let mut w = $crate::macros::Writer::new();
        write!(&mut w, $fmt, $($val),*).expect("Failed to format");
        let s: alloc::string::String = w.into();
        $crate::syscall::debug(&s);
    }};
}

#[macro_export]
macro_rules! define_entry {
    () => {
        #[no_mangle]
        pub extern "C" fn _start(init_port: $crate::zion::z_cap_t) -> ! {
            $crate::init::parse_init_port(init_port);
            $crate::mem::init_heap();

            let resp = main();

            $crate::syscall::process_exit(resp);
        }
    };
}
