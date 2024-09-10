use alloc::boxed::Box;
use core::{ffi::c_void, mem::MaybeUninit};

use mammoth::{mem::MemoryRegion, thread::Thread, zion::ZError};

use super::{hba::AhciHba, port::AhciPortHba};

#[derive(Debug)]
#[repr(C, packed)]
pub struct PciDeviceHeader {
    pub vendor_id: u16,
    pub device_id: u16,
    pub command_reg: u16,
    pub status_reg: u16,
    pub revision: u8,
    pub prog_interface: u8,
    pub subclass: u8,
    pub class_code: u8,
    pub cache_line_size: u8,
    pub latency_timer: u8,
    pub header_type: u8,
    pub bist: u8,
    pub bars: [u32; 5],
    pub abar: u32,
    __: u32,
    pub subsystem_id: u32,
    pub expansion_rom: u32,
    pub cap_ptr: u8,
    ___: [u8; 7],
    pub interrupt_line: u8,
    pub interrupt_pin: u8,
    pub min_grant: u8,
    pub max_latency: u8,
}

pub struct AhciController {
    pci_memory: MemoryRegion,
    hba_memory: MemoryRegion,
    irq_port: Option<mammoth::port::PortServer>,
    irq_thread: Option<Box<Thread>>,
    ports: [Option<PortController<'static>>; 32],
}

impl AhciController {
    pub fn new(pci_memory: MemoryRegion) -> Self {
        let pci_device_header = unsafe {
            pci_memory
                .mut_slice::<u8>()
                .as_mut_ptr()
                .cast::<PciDeviceHeader>()
                .as_mut()
                .unwrap()
        };
        let hba_memory =
            MemoryRegion::direct_physical(pci_device_header.abar as u64, 0x1100).unwrap();
        let mut controller = Self {
            pci_memory,
            hba_memory,
            irq_port: None,
            irq_thread: None,
            ports: [const { None }; 32],
        };
        mammoth::debug!("{:?}", controller.pci_header());
        controller.init();
        controller
    }

    fn init(&mut self) {
        self.ahci_hba().global_host_control.with_hba_reset(true);

        loop {
            if !self.ahci_hba().global_host_control.hba_reset() {
                break;
            }
        }

        self.ahci_hba().global_host_control.with_ahci_enable(true);

        mammoth::syscall::thread_sleep(50).unwrap();

        self.register_irq();

        self.init_ports();
    }

    fn run_server(&self) -> Result<Box<Thread>, ZError> {
        let thread_entry = |server_ptr: *const c_void| {
            let server = unsafe {
                (server_ptr as *mut Self)
                    .as_mut()
                    .expect("Failed to convert to server")
            };
            server.irq_loop();
        };
        Thread::spawn(
            thread_entry,
            self as *const Self as *const core::ffi::c_void,
        )
    }

    fn register_irq(&mut self) {
        let irq_num = match self.pci_header().interrupt_pin {
            1 => mammoth::zion::kZIrqPci1,
            2 => mammoth::zion::kZIrqPci2,
            3 => mammoth::zion::kZIrqPci3,
            4 => mammoth::zion::kZIrqPci4,
            _ => panic!(
                "Unrecognized pci interrupt pin {}",
                self.pci_header().interrupt_pin
            ),
        };
        self.irq_port = Some(mammoth::port::PortServer::from_cap(
            mammoth::syscall::register_irq(irq_num).unwrap(),
        ));

        self.irq_thread = Some(self.run_server().unwrap());

        self.ahci_hba()
            .global_host_control
            .with_interrupt_enable(true);
    }

    fn irq_loop(&self) {}

    fn init_ports(&mut self) {
        for i in 0..(self.ahci_hba().capabilities.num_ports() as usize) {
            let port_index = 1 << i;
            if (self.ahci_hba().port_implemented & port_index) != port_index {
                mammoth::debug!("Skipping port {}, not implemented", i);
                continue;
            }

            let port_offset: usize = 0x100 + (0x80 * i);
            let port_size = size_of::<AhciPortHba>();
            let port_limit = port_offset + port_size;
            let port = unsafe {
                self.hba_memory.mut_slice::<u8>()[port_offset..port_limit]
                    .as_mut_ptr()
                    .cast::<AhciPortHba>()
                    .as_mut()
                    .unwrap()
            };

            self.ports[i] = Some(PortController::new(port));
            self.ports[i].as_ref().unwrap().identify();
            mammoth::debug!("Identifying port {}", i);
        }
    }

    pub fn pci_header(&self) -> &mut PciDeviceHeader {
        unsafe {
            self.pci_memory
                .mut_slice::<u8>()
                .as_mut_ptr()
                .cast::<PciDeviceHeader>()
                .as_mut()
                .unwrap()
        }
    }

    pub fn ahci_hba(&self) -> &mut AhciHba {
        unsafe {
            self.hba_memory
                .mut_slice::<u8>()
                .as_mut_ptr()
                .cast::<AhciHba>()
                .as_mut()
                .unwrap()
        }
    }
}

struct PortController<'a> {
    ahci_port_hba: &'a AhciPortHba,
}

impl<'a> PortController<'a> {
    fn new(ahci_port_hba: &'a AhciPortHba) -> Self {
        Self { ahci_port_hba }
    }

    pub fn identify(&self) {}
}
