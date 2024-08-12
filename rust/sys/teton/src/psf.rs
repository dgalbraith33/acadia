use mammoth::debug;
use mammoth::zion::ZError;
use victoriafalls::file::File;

const MAGIC_HEADER: u32 = 0x864AB572;

#[repr(C)]
struct PsfHeader {
    magic: u32,           /* magic bytes to identify PSF */
    version: u32,         /* zero */
    headersize: u32,      /* offset of bitmaps in file, 32 */
    flags: u32,           /* 0 if there's no unicode table */
    numglyph: u32,        /* number of glyphs */
    bytes_per_glyph: u32, /* size of each glyph */
    height: u32,          /* height in pixels */
    width: u32,           /* width in pixels */
}

pub struct Psf {
    file: File,
    header: &'static PsfHeader,
}

impl Psf {
    pub fn new(path: &str) -> Result<Self, ZError> {
        let file = File::open(&path)?;

        let header = file
            .slice(0, core::mem::size_of::<PsfHeader>())
            .as_ptr()
            .cast();
        let psf = Self {
            file,
            header: unsafe { &*header },
        };

        psf.validate()?;

        Ok(psf)
    }

    fn validate(&self) -> Result<(), ZError> {
        if self.header.magic != MAGIC_HEADER {
            debug!("PSF: Magic value: {:x}", self.header.magic);
            return Err(ZError::INVALID_ARGUMENT);
        }

        if self.header.version != 0 {
            debug!("PSF non-zero version");
            return Err(ZError::INVALID_ARGUMENT);
        }

        if self.header.height != 0x10 {
            debug!("PSF height other than 16 not handled");
            return Err(ZError::UNIMPLEMENTED);
        }

        if self.header.width != 0x8 {
            debug!("PSF width other than 8 not handled");
            return Err(ZError::UNIMPLEMENTED);
        }
        Ok(())
    }

    pub fn glyph(&self, index: u32) -> &[u8] {
        let offset: usize =
            (self.header.headersize + (index * self.header.bytes_per_glyph)) as usize;
        let len: usize = self.header.bytes_per_glyph as usize;
        &self.file.slice(offset, len)
    }
}
