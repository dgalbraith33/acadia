#pragma once

#include <mammoth/channel.h>
#include <mammoth/memory_region.h>

class DenaliClient {
 public:
  DenaliClient(const Channel& channel) : channel_(channel) {}

  MappedMemoryRegion ReadSectors(uint64_t device_id, uint64_t lba,
                                 uint64_t num_sectors);

 private:
  Channel channel_;
};
