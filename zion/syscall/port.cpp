#include "syscall/port.h"

#include "interrupt/interrupt.h"
#include "scheduler/scheduler.h"
#include "syscall/syscall.h"

z_err_t PortCreate(ZPortCreateReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port = MakeRefCounted<Port>();
  *req->port_cap = proc.AddNewCapability(port, ZC_WRITE | ZC_READ);
  return Z_OK;
}

z_err_t PortSend(ZPortSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCap(port_cap, ZC_WRITE));

  auto port = port_cap->obj<Port>();
  RET_IF_NULL(port);
  ZMessage message{
      .num_bytes = req->num_bytes,
      .data = const_cast<void*>(req->data),
      .num_caps = req->num_caps,
      .caps = req->caps,
  };
  return port->Write(message);
}

z_err_t PortRecv(ZPortRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCap(port_cap, ZC_READ));

  auto port = port_cap->obj<Port>();
  RET_IF_NULL(port);
  ZMessage message{
      .num_bytes = *req->num_bytes,
      .data = const_cast<void*>(req->data),
      .num_caps = *req->num_caps,
      .caps = req->caps,
  };
  RET_ERR(port->Read(message));
  *req->num_bytes = message.num_bytes;
  *req->num_caps = message.num_caps;
  return Z_OK;
}

z_err_t PortPoll(ZPortPollReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCap(port_cap, ZC_READ));

  auto port = port_cap->obj<Port>();
  RET_IF_NULL(port);
  if (!port->HasMessages()) {
    return Z_ERR_EMPTY;
  }
  ZMessage message{
      .num_bytes = *req->num_bytes,
      .data = const_cast<void*>(req->data),
      .num_caps = *req->num_caps,
      .caps = req->caps,
  };
  RET_ERR(port->Read(message));
  *req->num_bytes = message.num_bytes;
  *req->num_caps = message.num_caps;
  return Z_OK;
}

z_err_t IrqRegister(ZIrqRegisterReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  if (req->irq_num != Z_IRQ_PCI_BASE) {
    // FIXME: Don't hardcode this nonsense.
    return Z_ERR_UNIMPLEMENTED;
  }
  RefPtr<Port> port = MakeRefCounted<Port>();
  *req->port_cap = proc.AddNewCapability(port, ZC_READ | ZC_WRITE);
  RegisterPciPort(port);
  return Z_OK;
}
