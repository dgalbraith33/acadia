use alloc::boxed::Box;
use alloc::rc::Rc;
use core::ffi::c_void;

use mammoth::{mem::MemoryRegion, thread::Thread, zion::ZError};

use crate::ahci::command::FisType;
use crate::ahci::port::{
    AhciDeviceDetection, AhciInterfacePowerManagement, AhciPortInterruptStatus,
};

use super::command::{
    CommandList, CommandTable, HostToDeviceRegisterFis, ReceivedFis, SataCommand,
};
use super::{
    hba::AhciHba,
    port::{AhciPortHba, AhciPortInterruptEnable, AhciSataError},
};

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
        controller.init();
        controller
    }

    pub fn join(&self) -> Result<(), ZError> {
        self.irq_thread.as_ref().unwrap().join()
    }

    fn init(&mut self) {
        self.ahci_hba().global_host_control.set_hba_reset(true);

        mammoth::syscall::thread_sleep(50).unwrap();

        loop {
            if !self.ahci_hba().global_host_control.hba_reset() {
                break;
            }
        }

        self.ahci_hba().global_host_control.set_ahci_enable(true);

        mammoth::syscall::thread_sleep(50).unwrap();

        self.register_irq();

        self.init_ports().unwrap();
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
            .set_interrupt_enable(true);
    }

    fn irq_loop(&mut self) {
        loop {
            self.irq_port.as_ref().unwrap().recv_null().unwrap();

            for i in 0..self.ahci_hba().capabilities.num_ports() {
                let int_offset = 1 << i;
                if (self.ahci_hba().interrupt_status & int_offset) == int_offset {
                    if let Some(port) = &mut self.ports[i as usize] {
                        port.handle_interrupt();
                        self.ahci_hba().interrupt_status &= !int_offset;
                    }
                }
            }
        }
    }

    fn init_ports(&mut self) -> Result<(), ZError> {
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

            let sata_status = port.sata_status;
            if (sata_status.device_detection() != AhciDeviceDetection::CommunicationEstablished)
                || (sata_status.interface_power_management()
                    != AhciInterfacePowerManagement::Active)
            {
                mammoth::debug!(
                    "Skipping port {}, no communcation. Status: {:?}",
                    i,
                    sata_status
                );
                continue;
            }

            self.ports[i] = Some(PortController::new(port)?);
            self.ports[i].as_mut().unwrap().identify()?;
        }
        Ok(())
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

struct Command {
    command: SataCommand,
    lba: u64,
    sector_cnt: u16,
    paddr: u64,

    #[allow(dead_code)] // We need to own this even if we never access it.
    memory_region: MemoryRegion,
}

impl Command {
    pub fn identify() -> Result<Self, ZError> {
        let (memory_region, paddr) = MemoryRegion::contiguous_physical(512)?;

        Ok(Self {
            command: SataCommand::IdentifyDevice,
            lba: 0,
            sector_cnt: 1,
            paddr,
            memory_region,
        })
    }
}

impl From<&Command> for HostToDeviceRegisterFis {
    fn from(val: &Command) -> Self {
        HostToDeviceRegisterFis::new_command(val.command, val.lba, val.sector_cnt)
    }
}

struct PortController<'a> {
    ahci_port_hba: &'a mut AhciPortHba,
    command_slots: [Option<Rc<Command>>; 32],
    command_structures: MemoryRegion,
    command_paddr: u64,
}

impl<'a> PortController<'a> {
    fn new(ahci_port_hba: &'a mut AhciPortHba) -> Result<Self, ZError> {
        let sata_status = ahci_port_hba.sata_status;
        assert_eq!(
            sata_status.device_detection(),
            AhciDeviceDetection::CommunicationEstablished
        );
        assert_eq!(
            sata_status.interface_power_management(),
            AhciInterfacePowerManagement::Active,
        );
        let (command_structures, command_paddr) = MemoryRegion::contiguous_physical(0x2500)?;
        ahci_port_hba.command_list_base = command_paddr;
        ahci_port_hba.fis_base = command_paddr + 0x400;

        ahci_port_hba.interrupt_enable = AhciPortInterruptEnable::from_bits(0xFFFF_FFFF);
        // Overwrite all errors.
        ahci_port_hba.sata_error = AhciSataError::from_bits(0xFFFF_FFFF);

        let command = ahci_port_hba.command;
        ahci_port_hba.command = command.with_fis_recieve_enable(true).with_start(true);

        let mut controller = Self {
            ahci_port_hba,
            command_slots: [const { None }; 32],
            command_structures,
            command_paddr,
        };

        // This leaves space for 8 prdt entries.
        for i in 0..32 {
            controller.command_list()[i].command_table_base_addr =
                (command_paddr + 0x500) + (0x100 * (i as u64));
        }

        Ok(controller)
    }

    pub fn identify(&mut self) -> Result<(), ZError> {
        if self.ahci_port_hba.signature == 0x101 {
            self.issue_command(Rc::from(Command::identify()?))?;
        } else {
            let sig = self.ahci_port_hba.signature;
            mammoth::debug!("Skipping non-sata sig: {:#0x}", sig);
        }
        Ok(())
    }

    fn issue_command(&mut self, command: Rc<Command>) -> Result<(), ZError> {
        let slot = self.select_slot()?;
        self.command_slots[slot] = Some(command.clone());

        self.command_tables()[slot].command_fis.host_to_device = command.clone().as_ref().into();

        self.command_tables()[slot].prdt[0].region_address = command.paddr;
        self.command_tables()[slot].prdt[0].byte_count = 512 * (command.sector_cnt as u32);

        self.command_list()[slot].prd_table_length = 1;

        self.command_list()[slot].command =
            (size_of::<HostToDeviceRegisterFis>() as u16 / 4) & 0x1F;
        self.command_list()[slot].command |= 1 << 7;
        self.ahci_port_hba.command_issue |= 1 << slot;
        Ok(())
    }

    fn select_slot(&self) -> Result<usize, ZError> {
        for i in 0..self.command_slots.len() {
            match self.command_slots[i] {
                None => return Ok(i),
                _ => {}
            }
        }
        return Err(ZError::EXHAUSTED);
    }

    fn command_list(&mut self) -> &mut CommandList {
        unsafe {
            self.command_structures
                .mut_slice::<u8>()
                .as_mut_ptr()
                .cast::<CommandList>()
                .as_mut()
                .unwrap()
        }
    }

    fn recieved_fis(&mut self) -> &mut ReceivedFis {
        unsafe {
            self.command_structures.mut_slice::<u8>()[0x400..]
                .as_mut_ptr()
                .cast::<ReceivedFis>()
                .as_mut()
                .unwrap()
        }
    }

    fn command_tables(&mut self) -> &mut [CommandTable; 32] {
        unsafe {
            self.command_structures.mut_slice::<u8>()[0x500..]
                .as_mut_ptr()
                .cast::<[CommandTable; 32]>()
                .as_mut()
                .unwrap()
        }
    }

    fn handle_interrupt(&mut self) {
        let int_status = self.ahci_port_hba.interrupt_status;
        if int_status.device_to_host_register_fis_interrupt() {
            assert_eq!(
                self.recieved_fis().device_to_host_register_fis.fis_type as u8,
                FisType::RegisterDeviceToHost as u8
            );
            if self.recieved_fis().device_to_host_register_fis.error != 0 {
                mammoth::debug!(
                    "D2H err: {:#0x}",
                    self.recieved_fis().device_to_host_register_fis.error
                );

                mammoth::debug!(
                    "Status: {:#0x}",
                    self.recieved_fis().device_to_host_register_fis.status
                );
            }

            self.ahci_port_hba.interrupt_status =
                AhciPortInterruptStatus::new().with_device_to_host_register_fis_interrupt(true);
        }
        if int_status.pio_setup_fis_interrupt() {
            self.ahci_port_hba.interrupt_status =
                AhciPortInterruptStatus::new().with_pio_setup_fis_interrupt(true);
        }

        for i in 0..32 {
            let int_offset = 1 << i;

            // If there is no longer a command issued on a slot and we have something in
            // the command list we know that this is the command that finished.
            // FIXME: This could cause a race condition when issuing a command if a different
            // interrupt triggers between us setting the command in the command slot and
            // actually issuing the command.
            if (self.ahci_port_hba.command_issue & int_offset) != int_offset {
                if let Some(_) = &self.command_slots[i] {
                    self.finish_command(i);
                    self.command_slots[i] = None;
                }
            }
        }
    }

    fn finish_command(&self, slot: usize) {
        mammoth::debug!("Finishing command in slot {}", slot);
    }
}
