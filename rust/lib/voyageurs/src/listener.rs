use core::cell::RefCell;

use alloc::boxed::Box;
use alloc::rc::Rc;
use alloc::string::ToString;
use mammoth::cap::Capability;
use mammoth::port::PortServer;
use mammoth::thread::Thread;
use mammoth::zion::ZError;

#[repr(u8)]
#[derive(PartialEq, Eq, PartialOrd, Ord, Copy, Clone)]
enum Keycode {
    UnknownKeycode = 0x0,

    A = 0x1,
    B = 0x2,
    C = 0x3,
    D = 0x4,
    E = 0x5,
    F = 0x6,
    G = 0x7,
    H = 0x8,
    I = 0x9,
    J = 0xA,
    K = 0xB,
    L = 0xC,
    M = 0xD,
    N = 0xE,
    O = 0xF,
    P = 0x10,
    Q = 0x11,
    R = 0x12,
    S = 0x13,
    T = 0x14,
    U = 0x15,
    V = 0x16,
    W = 0x17,
    X = 0x18,
    Y = 0x19,
    Z = 0x1A,

    NUM1 = 0x20,
    NUM2 = 0x21,
    NUM3 = 0x22,
    NUM4 = 0x23,
    NUM5 = 0x24,
    NUM6 = 0x25,
    NUM7 = 0x26,
    NUM8 = 0x27,
    NUM9 = 0x28,
    NUM0 = 0x29,

    Space = 0x30,
    Enter = 0x31,
    Tab = 0x32,
    Backspace = 0x33,
    Delete = 0x34,

    Minus = 0x40,
    Equals = 0x41,
    LBrace = 0x42,
    RBrace = 0x43,
    BSlash = 0x44,
    FSlash = 0x45,
    Semicolon = 0x46,
    Quote = 0x47,
    Comma = 0x48,
    Period = 0x49,
    Backtick = 0x4A,

    LShift = 0x50,
    RShift = 0x51,
    LCtrl = 0x52,
    RCtrl = 0x53,
    LAlt = 0x54,
    RAlt = 0x55,
    Super = 0x56,
    Esc = 0x57,
    Up = 0x58,
    Down = 0x59,
    Left = 0x5A,
    Right = 0x5B,
}

impl Keycode {
    fn from_scancode(scancode: u16) -> Self {
        match scancode as u8 {
            0x04 => Keycode::A,
            0x05 => Keycode::B,
            0x06 => Keycode::C,
            0x07 => Keycode::D,
            0x08 => Keycode::E,
            0x09 => Keycode::F,
            0x0A => Keycode::G,
            0x0B => Keycode::H,
            0x0C => Keycode::I,
            0x0D => Keycode::J,
            0x0E => Keycode::K,
            0x0F => Keycode::L,
            0x10 => Keycode::M,
            0x11 => Keycode::N,
            0x12 => Keycode::O,
            0x13 => Keycode::P,
            0x14 => Keycode::Q,
            0x15 => Keycode::R,
            0x16 => Keycode::S,
            0x17 => Keycode::T,
            0x18 => Keycode::U,
            0x19 => Keycode::V,
            0x1A => Keycode::W,
            0x1B => Keycode::X,
            0x1C => Keycode::Y,
            0x1D => Keycode::Z,
            0x1E => Keycode::NUM1,
            0x1F => Keycode::NUM2,
            0x20 => Keycode::NUM3,
            0x21 => Keycode::NUM4,
            0x22 => Keycode::NUM5,
            0x23 => Keycode::NUM6,
            0x24 => Keycode::NUM7,
            0x25 => Keycode::NUM8,
            0x26 => Keycode::NUM9,
            0x27 => Keycode::NUM0,
            0x28 => Keycode::Enter,
            0x29 => Keycode::Esc,
            0x2A => Keycode::Backspace,
            0x2B => Keycode::Tab,
            0x2C => Keycode::Space,
            0x2D => Keycode::Minus,
            0x2E => Keycode::Equals,
            0x2F => Keycode::LBrace,
            0x30 => Keycode::RBrace,
            0x31 => Keycode::BSlash,
            0x33 => Keycode::Semicolon,
            0x34 => Keycode::Quote,
            0x35 => Keycode::Backtick,
            0x36 => Keycode::Comma,
            0x37 => Keycode::Period,
            0x38 => Keycode::FSlash,
            0x39 => Keycode::Esc,
            _ => Keycode::UnknownKeycode,
        }
    }
}

struct Modifiers(u8);

impl Modifiers {
    fn from_scancode(scancode: u16) -> Self {
        Self((scancode >> 8) as u8)
    }

    fn is_shift(&self) -> bool {
        ((self.0 & 0x20) == 0x20) || ((self.0 & 0x2) == 0x2)
    }
}

fn into_char(keycode: Keycode, modifiers: Modifiers) -> char {
    match keycode {
        k if (Keycode::A..=Keycode::Z).contains(&k) => {
            if modifiers.is_shift() {
                let chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                chars.as_bytes()[k as usize - Keycode::A as usize] as char
            } else {
                let chars = "abcdefghijklmnopqrstuvwxyz";
                chars.as_bytes()[k as usize - Keycode::A as usize] as char
            }
        }
        k if (Keycode::NUM1..=Keycode::NUM0).contains(&k) => {
            if modifiers.is_shift() {
                let chars = "!@#$%^&*()";
                chars.as_bytes()[k as usize - Keycode::NUM1 as usize] as char
            } else {
                let chars = "12345687890";
                chars.as_bytes()[k as usize - Keycode::NUM1 as usize] as char
            }
        }
        k if (Keycode::Minus..=Keycode::Backtick).contains(&k) => {
            if modifiers.is_shift() {
                let chars = "_+{}|?:\"<>~";
                chars.as_bytes()[k as usize - Keycode::Minus as usize] as char
            } else {
                let chars = "-=[]\\/;',.`";
                chars.as_bytes()[k as usize - Keycode::Minus as usize] as char
            }
        }
        Keycode::Enter => '\n',
        Keycode::Space => ' ',
        Keycode::Tab => '\t',
        Keycode::Backspace => '\x08',
        _ => '\0',
    }
}

pub trait KeyboardHandler {
    fn handle_char(&mut self, c: char);
}

pub struct KeyboardListener {
    listen_port: PortServer,
    listen_thread: Option<Box<Thread>>,
    handler: Rc<RefCell<dyn KeyboardHandler>>,
}

impl KeyboardListener {
    pub fn new(handler: Rc<RefCell<dyn KeyboardHandler>>) -> Result<Box<Self>, ZError> {
        let mut listnr = Box::new(Self {
            listen_port: PortServer::new()?,
            listen_thread: None,
            handler,
        });

        let voyageur_endpoint = yellowstone::from_init_endpoint()
            .get_endpoint(&yellowstone::GetEndpointRequest {
                endpoint_name: "voyageurs".to_string(),
            })?
            .endpoint;

        let mut voyageur_client = crate::VoyageursClient::new(Capability::take(voyageur_endpoint));

        voyageur_client.register_keyboard_listener(&crate::KeyboardListener {
            port_capability: listnr.listen_port.create_client_cap()?,
        })?;

        let thread_entry = |self_raw| {
            let listener = unsafe {
                (self_raw as *mut KeyboardListener)
                    .as_mut()
                    .expect("Failed to convert to keyboard listener")
            };
            listener.listen_loop();
        };

        listnr.listen_thread = Some(Thread::spawn(
            thread_entry,
            &*listnr as *const Self as *const core::ffi::c_void,
        )?);

        Ok(listnr)
    }

    fn listen_loop(&mut self) {
        loop {
            let scancode = self
                .listen_port
                .recv_u16()
                .expect("Failed to recieve scancode");

            let keycode = Keycode::from_scancode(scancode);
            let modifiers = Modifiers::from_scancode(scancode);

            self.handler
                .as_ref()
                .borrow_mut()
                .handle_char(into_char(keycode, modifiers))
        }
    }

    pub fn join(&self) -> Result<(), ZError> {
        self.listen_thread.as_ref().unwrap().join()
    }
}
