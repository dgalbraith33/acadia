#include "object/endpoint.h"

#include "scheduler/scheduler.h"

glcr::RefPtr<Endpoint> Endpoint::Create() {
  return glcr::AdoptPtr(new Endpoint);
}
