#include "interrupt/timer.h"

#include "common/port.h"

namespace {

// IO Ports
constexpr uint8_t kPit0Ctl = 0x40;
constexpr uint8_t kPit1Ctl = 0x41;
constexpr uint8_t kPit2Ctl = 0x42;
constexpr uint8_t kPitCmd = 0x43;

// Command Register
// Mode
constexpr uint8_t kPitMode0 = 0x00;
constexpr uint8_t kPitMode1 = 0x02;
constexpr uint8_t kPitMode2 = 0x04;
constexpr uint8_t kPitMode3 = 0x06;
constexpr uint8_t kPitMode4 = 0x08;
constexpr uint8_t kPitMode5 = 0x0A;
// R/W
constexpr uint8_t kCmdLatch = 0x00;
constexpr uint8_t kCmdRwLow = 0x10;
constexpr uint8_t kCmdRwHi = 0x20;
constexpr uint8_t kCmdRwBoth = 0x30;
// PIT Select
constexpr uint8_t kSelect0 = 0x00;
constexpr uint8_t kSelect1 = 0x40;
constexpr uint8_t kSelect2 = 0x80;
constexpr uint8_t kReadback = 0xC0;

constexpr uint32_t kPitFrequency = 1193182;

}  // namespace

void SetFrequency(uint64_t hertz) {
  uint16_t reload = kPitFrequency / hertz;
  outb(kPitCmd, kPitMode3 | kCmdRwBoth | kSelect0);
  outb(kPit0Ctl, reload & 0xFF);
  outb(kPit0Ctl, reload >> 8);
}
