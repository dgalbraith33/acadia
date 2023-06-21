#include "syscall/port.h"

#include "capability/capability.h"
#include "interrupt/interrupt.h"
#include "scheduler/scheduler.h"

z_err_t PortCreate(ZPortCreateReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port = glcr::MakeRefCounted<Port>();
  *req->port_cap = proc.AddNewCapability(port, ZC_WRITE | ZC_READ);
  return Z_OK;
}

z_err_t PortSend(ZPortSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCapability<Port>(port_cap, ZC_WRITE));

  auto port = port_cap->obj<Port>();
  return port->Write(req->num_bytes, req->data, req->num_caps, req->caps);
}

z_err_t PortRecv(ZPortRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCapability<Port>(port_cap, ZC_READ));

  auto port = port_cap->obj<Port>();
  ZMessage message{
      .num_bytes = *req->num_bytes,
      .data = const_cast<void*>(req->data),
      .num_caps = *req->num_caps,
      .caps = req->caps,
  };
  return port->Read(req->num_bytes, req->data, req->num_caps, req->caps);
}

z_err_t PortPoll(ZPortPollReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCapability<Port>(port_cap, ZC_READ));

  auto port = port_cap->obj<Port>();
  // FIXME: Race condition here where this call could block if the last message
  // is removed between this check and the port read.
  if (!port->HasMessages()) {
    return Z_ERR_EMPTY;
  }
  return port->Read(req->num_bytes, req->data, req->num_caps, req->caps);
}

z_err_t IrqRegister(ZIrqRegisterReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  if (req->irq_num != Z_IRQ_PCI_BASE) {
    // FIXME: Don't hardcode this nonsense.
    return Z_ERR_UNIMPLEMENTED;
  }
  glcr::RefPtr<Port> port = glcr::MakeRefCounted<Port>();
  *req->port_cap = proc.AddNewCapability(port, ZC_READ | ZC_WRITE);
  RegisterPciPort(port);
  return Z_OK;
}
