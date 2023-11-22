#include "sync/semaphore.h"

#include <zcall.h>

#include "mammoth/debug.h"

Semaphore::Semaphore() { check(ZSemaphoreCreate(&semaphore_cap_)); }
Semaphore::~Semaphore() { check(ZCapRelease(semaphore_cap_)); }

void Semaphore::Wait() { check(ZSemaphoreWait(semaphore_cap_)); }
void Semaphore::Signal() { check(ZSemaphoreSignal(semaphore_cap_)); }
