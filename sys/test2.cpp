#include <mammoth/channel.h>
#include <mammoth/debug.h>
#include <mammoth/thread.h>

void thread_entry(void* a) {
  dbgln("In thread");
  dbgln(static_cast<const char*>(a));
}

int main(uint64_t bootstrap_cap) {
  dbgln("Main thread");

  const char* a = "a";
  const char* b = "bee";
  const char* c = "cee";
  const char* d = "dee";
  Thread t1(thread_entry, a);
  Thread t2(thread_entry, b);

  uint64_t size = 10;
  char buff[10];
  Channel c1;
  c1.adopt_cap(bootstrap_cap);
  check(c1.ReadStr(buff, &size));
  dbgln(buff);
  return 0;
}
