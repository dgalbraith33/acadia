use crate::debug;
use crate::init;
use crate::syscall;
use crate::zion::z_cap_t;
use crate::zion::ZError;

use alloc::fmt::Debug;

const ELF_MAGIC: u32 = 0x464C457F;

const ELF_IDENT_32BIT: u8 = 0x1;
const ELF_IDENT_64BIT: u8 = 0x2;

const ELF_ENDIAN_LITTLE: u8 = 0x1;
const ELF_ENDIAN_BIG: u8 = 0x2;

const ELF_VERSION_CURRENT: u8 = 0x1;

const ELF_ABI_SYSV: u8 = 0x0;
const ELF_ABI_LINUX: u8 = 0x3;

const ELF_FILE_RELOC: u16 = 0x1;
const ELF_FILE_EXEC: u16 = 0x2;
const ELF_FILE_DYN: u16 = 0x3;
const ELF_FILE_CORE: u16 = 0x4;

const ELF_MACH_X86: u16 = 0x3;
const ELF_MACH_AMD64: u16 = 0x3e;

#[repr(C, packed(1))]
// Header spec from wikipedia: https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#ELF_header
struct Elf64Header {
    // 0x7F454C46 (0x7F followed by 'ELF')
    magic: u32,

    // 1 for 32 bit, 2 for 64 bit.
    elf_class: u8,

    // 1 for little, 2 for big.
    endianess: u8,

    // Current version is 1.
    ident_version: u8,

    // Target OS abi.
    abi: u8,
    abi_version: u8,

    ident_padding: [u8; 7],

    file_type: u16,
    machine: u16,
    version: u32,
    entry: u64,
    program_header_offset: u64,
    section_header_offset: u64,
    flags: u32,
    header_size: u16,
    program_header_entry_size: u16,
    program_header_count: u16,
    section_header_entry_size: u16,
    section_header_entry_number: u16,
    section_header_str_index: u16,
}

impl Debug for Elf64Header {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        let magic = self.magic;
        let elf_class = match self.elf_class {
            ELF_IDENT_32BIT => "32 bit",
            ELF_IDENT_64BIT => "64 bit",
            _ => "Unknown",
        };
        let endianess = match self.endianess {
            ELF_ENDIAN_LITTLE => "Little",
            ELF_ENDIAN_BIG => "Big",
            _ => "Unknown",
        };

        let ident_version = self.ident_version;
        let version = self.version;

        f.write_fmt(format_args!(
            "ELF Header Magic: {:#x}, Class: {}, Endianess: {}, Version (ident): {}, Version: {}\n",
            magic, elf_class, endianess, ident_version, version,
        ))?;

        let abi = match self.abi {
            ELF_ABI_SYSV => "SYSV",
            ELF_ABI_LINUX => "Linux",
            _ => "Unknown",
        };
        let abi_version = self.abi_version;

        f.write_fmt(format_args!("\tABI: {}, Version: {}\n", abi, abi_version))?;

        let file_type = match self.file_type {
            ELF_FILE_EXEC => "Executable",
            ELF_FILE_RELOC => "Relocatable",
            ELF_FILE_DYN => "Shared Obj",
            ELF_FILE_CORE => "Core file",
            _ => "Unknown",
        };

        let machine = match self.machine {
            ELF_MACH_X86 => "x86 (32bit)",
            ELF_MACH_AMD64 => "x86-64",
            _ => "Unknown",
        };

        let entry_point = self.entry;

        f.write_fmt(format_args!(
            "\tFile type: {}, Machine Arch: {}, Entry point {:#x}",
            file_type, machine, entry_point
        ))
    }
}

fn validate_header(elf_header: &Elf64Header) -> Result<(), ZError> {
    if elf_header.magic != ELF_MAGIC {
        let magic = elf_header.magic;
        debug!(
            "Elf header incorrect got {:#x} expected {:#x}",
            magic, ELF_MAGIC
        );
        return Err(ZError::INVALID_ARGUMENT);
    }

    if elf_header.elf_class != ELF_IDENT_64BIT {
        let class = elf_header.elf_class;
        debug!(
            "Elf class must be {} for 64 bit, got: {}",
            ELF_IDENT_64BIT, class
        );
        return Err(ZError::INVALID_ARGUMENT);
    }

    if elf_header.endianess != ELF_ENDIAN_LITTLE {
        let endianess = elf_header.endianess;
        debug!(
            "Elf endianess must be {} for little, got: {}",
            ELF_ENDIAN_LITTLE, endianess
        );
        return Err(ZError::INVALID_ARGUMENT);
    }

    if elf_header.ident_version != ELF_VERSION_CURRENT {
        let version = elf_header.ident_version;
        debug!(
            "Elf version (ident) must be {}, got: {}",
            ELF_VERSION_CURRENT, version
        );
        return Err(ZError::INVALID_ARGUMENT);
    }

    if elf_header.file_type != ELF_FILE_EXEC {
        let file_type = elf_header.file_type;
        debug!(
            "Elf file type must be {} for executable, got {:x}",
            ELF_FILE_EXEC, file_type
        );
        return Err(ZError::INVALID_ARGUMENT);
    }

    Ok(())
}

const ELF_PROG_NULL: u32 = 0x0;
const ELF_PROG_LOAD: u32 = 0x1;
const ELF_PROG_DYNAMIC: u32 = 0x2;
const ELF_PROG_INTERP: u32 = 0x3;
const ELF_PROG_NOTE: u32 = 0x4;
const ELF_PROG_SHLIB: u32 = 0x5;
const ELF_PROG_PTHDR: u32 = 0x6;
const ELF_PROG_THREAD_LOCAL: u32 = 0x7;

// Stack unwind tables.
const ELF_PROG_GNU_EH_FRAME: u32 = 0x6474e550;
const ELF_PROG_GNU_STACK: u32 = 0x6474e551;
const ELF_PROG_GNU_RELRO: u32 = 0x6474e552;

#[repr(C, packed(1))]
struct Elf64ProgramHeader {
    prog_type: u32,
    flags: u32,
    offset: u64,
    vaddr: u64,
    paddr: u64,
    file_size: u64,
    mem_size: u64,
    align: u64,
}

impl Debug for Elf64ProgramHeader {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        let prog_type = match self.prog_type {
            ELF_PROG_NULL => "NULL",
            ELF_PROG_LOAD => "LOAD",
            ELF_PROG_DYNAMIC => "DYNAMIC",
            ELF_PROG_INTERP => "INTERP",
            ELF_PROG_NOTE => "NOTE",
            ELF_PROG_SHLIB => "SHARED LIB",
            ELF_PROG_PTHDR => "PROG TABLE HEADER",
            ELF_PROG_THREAD_LOCAL => "THREAD LOCAL",
            ELF_PROG_GNU_EH_FRAME => "GNU EH FRAME",
            ELF_PROG_GNU_RELRO => "GNU RELOCATABLE",
            ELF_PROG_GNU_STACK => "GNU STACK",
            _ => "UNKNOWN",
        };

        let offset = self.offset;
        let vaddr = self.vaddr;
        let paddr = self.paddr;
        let file_size = self.file_size;
        let mem_size = self.mem_size;
        let align = self.align;
        f.write_fmt(format_args!(
            "Type: {}, offset: {:#x}, vaddr: {:#x}, paddr: {:#x}, file_size: {:#x}, mem_size: {:#x}, align: {:#x}",
            prog_type, offset, vaddr, paddr, file_size, mem_size, align
        ))
    }
}

fn load_program_segment(
    prog_header: &Elf64ProgramHeader,
    file: &[u8],
    vmas: z_cap_t,
) -> Result<(), ZError> {
    debug!("{:?}", prog_header);
    match prog_header.prog_type {
        ELF_PROG_NULL
        | ELF_PROG_NOTE
        | ELF_PROG_PTHDR
        | ELF_PROG_GNU_STACK
        | ELF_PROG_GNU_RELRO
        | ELF_PROG_GNU_EH_FRAME => Ok(()),
        ELF_PROG_LOAD => {
            let page_offset = prog_header.vaddr & 0xFFF;
            let mem_size = page_offset + prog_header.mem_size;

            let mem_object = crate::mem::MemoryRegion::new(mem_size)?;

            for i in mem_object.mut_slice() {
                *i = 0;
            }

            let file_start = prog_header.offset as usize;
            let file_end = file_start + prog_header.file_size as usize;
            let from_slice = &file[file_start..file_end];

            let mem_start = page_offset as usize;
            let mem_end = mem_start + (prog_header.file_size as usize);
            let to_slice: &mut [u8] = &mut mem_object.mut_slice()[mem_start..mem_end];

            to_slice.copy_from_slice(from_slice);

            let vaddr = prog_header.vaddr - page_offset;
            syscall::address_space_map_external(vmas, mem_object.cap(), vaddr)
        }
        ELF_PROG_DYNAMIC => {
            debug!("Unimplemented dynamic elf sections.");
            Err(ZError::UNIMPLEMENTED)
        }
        ELF_PROG_INTERP => {
            debug!("Unimplemented interpreter elf sections.");
            Err(ZError::UNIMPLEMENTED)
        }
        ELF_PROG_SHLIB => {
            debug!("Unimplemented shared lib elf sections.");
            Err(ZError::UNIMPLEMENTED)
        }
        ELF_PROG_THREAD_LOCAL => {
            debug!("Unimplemented thread local elf sections.");
            Err(ZError::UNIMPLEMENTED)
        }
        _ => {
            let prog_type = prog_header.prog_type;
            debug!("Unknown elf program header type: {:#x}", prog_type);
            Err(ZError::UNKNOWN)
        }
    }
}

fn load_elf_program(elf_file: &[u8], vmas: z_cap_t) -> Result<u64, ZError> {
    assert!(elf_file.len() > size_of::<Elf64Header>());
    let header: &Elf64Header = unsafe {
        elf_file
            .as_ptr()
            .cast::<Elf64Header>()
            .as_ref()
            .ok_or(ZError::NULL_PTR)?
    };
    debug!("{:?}", header);
    validate_header(header)?;

    for prog_ind in 0..header.program_header_count {
        let prog_header_offset = header.program_header_offset
            + ((prog_ind as u64) * (header.program_header_entry_size as u64));
        let prog_header_end = prog_header_offset + header.program_header_entry_size as u64;
        let prog_header_slice = &elf_file[prog_header_offset as usize..prog_header_end as usize];
        let prog_header: &Elf64ProgramHeader = unsafe {
            prog_header_slice
                .as_ptr()
                .cast::<Elf64ProgramHeader>()
                .as_ref()
                .ok_or(ZError::NULL_PTR)?
        };

        load_program_segment(prog_header, elf_file, vmas)?;
    }
    Ok(header.entry)
}

pub fn spawn_process_from_elf(elf_file: &[u8]) -> Result<z_cap_t, ZError> {
    let self_cap = unsafe { init::SELF_PROC_CAP };
    let port_cap = syscall::port_create()?;
    let port_cap_dup = syscall::cap_duplicate(port_cap, u64::MAX)?;

    let (new_proc_cap, new_as_cap, foreign_port_id) =
        syscall::process_spawn(self_cap, port_cap_dup)?;

    let entry_point = load_elf_program(elf_file, new_as_cap)?;

    let port = crate::port::PortClient::take_from(port_cap);

    port.write_u64_and_cap(
        crate::init::Z_INIT_SELF_PROC,
        syscall::cap_duplicate(new_proc_cap, u64::MAX)?,
    )?;
    port.write_u64_and_cap(crate::init::Z_INIT_SELF_VMAS, new_as_cap)?;
    port.write_u64_and_cap(
        crate::init::Z_INIT_ENDPOINT,
        self_cap.duplicate(Capability::PERMS_ALL)?,
    )?;

    let thread_cap = syscall::thread_create(new_proc_cap)?;
    syscall::thread_start(thread_cap, entry_point, foreign_port_id, 0)?;

    syscall::cap_release(thread_cap)?;

    Ok(new_proc_cap)
}
