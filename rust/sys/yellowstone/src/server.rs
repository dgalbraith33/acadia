use alloc::rc::Rc;
use mammoth::zion::ZError;
use yellowstone_yunq::{
    AhciInfo, DenaliInfo, Endpoint, FramebufferInfo, GetEndpointRequest, RegisterEndpointRequest,
    XhciInfo, YellowstoneServerHandler,
};

pub struct YellowstoneServerContext {
    denali_semaphore: mammoth::sync::Semaphore,
}

impl YellowstoneServerContext {
    pub fn new() -> Result<Self, ZError> {
        Ok(Self {
            denali_semaphore: mammoth::sync::Semaphore::new()?,
        })
    }

    pub fn wait_denali(&self) -> Result<(), ZError> {
        self.denali_semaphore.wait()
    }
}

pub struct YellowstoneServerImpl {
    context: Rc<YellowstoneServerContext>,
}

impl YellowstoneServerImpl {
    pub fn new(context: Rc<YellowstoneServerContext>) -> Self {
        Self { context }
    }
}

impl YellowstoneServerHandler for YellowstoneServerImpl {
    fn register_endpoint(&self, req: &RegisterEndpointRequest) -> Result<(), ZError> {
        todo!()
    }

    fn get_endpoint(&self, req: &GetEndpointRequest) -> Result<Endpoint, ZError> {
        todo!()
    }

    fn get_ahci_info(&self) -> Result<AhciInfo, ZError> {
        todo!()
    }

    fn get_xhci_info(&self) -> Result<XhciInfo, ZError> {
        todo!()
    }

    fn get_framebuffer_info(&self) -> Result<FramebufferInfo, ZError> {
        todo!()
    }

    fn get_denali(&self) -> Result<DenaliInfo, ZError> {
        todo!()
    }
}
