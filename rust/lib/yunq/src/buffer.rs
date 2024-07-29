use alloc::boxed::Box;
use mammoth::zion::ZError;

pub struct ByteBuffer<const N: usize> {
    buffer: Box<[u8; N]>,
}

impl<const N: usize> ByteBuffer<N> {
    pub fn new() -> Self {
        Self {
            buffer: Box::new([0; N]),
        }
    }
    pub fn size(&self) -> u64 {
        N as u64
    }

    pub fn slice(&self, len: usize) -> &[u8] {
        &self.buffer[..len]
    }

    pub fn mut_slice(&mut self) -> &mut [u8] {
        &mut self.buffer[..]
    }

    pub fn write_at<T>(&mut self, offset: usize, obj: T) -> Result<(), ZError> {
        if (size_of::<T>() + offset) > N {
            return Err(ZError::BUFFER_SIZE);
        }
        unsafe {
            *(self.buffer[offset..].as_mut_ptr() as *mut T) = obj;
        }
        Ok(())
    }

    pub fn write_str_at(&mut self, offset: usize, s: &str) -> Result<(), ZError> {
        if (s.len() + offset) > N {
            return Err(ZError::BUFFER_SIZE);
        }
        for i in 0..s.len() {
            self.buffer[offset + i] = s.as_bytes()[i];
        }
        Ok(())
    }

    pub fn at<T: Copy>(&self, offset: usize) -> Result<T, ZError> {
        if (size_of::<T>() + offset) > N {
            return Err(ZError::BUFFER_SIZE);
        }
        unsafe { Ok(*(self.buffer[offset..].as_ptr() as *const T)) }
    }

    pub fn str_at(&self, offset: usize, len: usize) -> Result<&str, ZError> {
        if (len + offset) > N {
            return Err(ZError::BUFFER_SIZE);
        }
        Ok(alloc::str::from_utf8(&self.buffer[offset..offset + len])
            .map_err(|_| ZError::INVALID_ARGUMENT)?)
    }
}
