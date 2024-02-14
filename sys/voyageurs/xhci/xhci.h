#pragma once

#include <stdint.h>

// TODO: Move to shared lib for denali and voyageurs.
struct PciDeviceHeader {
  uint16_t vendor_id;
  uint16_t device_id;
  uint16_t command_reg;
  uint16_t status_reg;
  uint8_t revision;
  uint8_t prog_interface;
  uint8_t subclass;
  uint8_t class_code;
  uint8_t cache_line_size;
  uint8_t latency_timer;
  uint8_t header_type;
  uint8_t bist;
  uint32_t bars[5];
  uint32_t abar;
  uint32_t reserved0;
  uint32_t subsystem_id;
  uint32_t expansion_rom;
  uint8_t cap_ptr;
  uint8_t reserved1[7];
  uint8_t interrupt_line;
  uint8_t interrupt_pin;
  uint8_t min_grant;
  uint8_t max_latency;
} __attribute__((packed));

struct XhciCapabilities {
  // NOTE: In qemu access these addresses at anything other than a 32bit offset
  // seems to give bogus values so we group the fields more than necessary.
  uint32_t length_and_version;
  uint32_t hcs_params_1;
  uint32_t hcs_params_2;
  uint32_t hcs_params_3;
  uint16_t capabilites;
  uint16_t ext_capabilities_pointer;
  uint32_t doorbell_offset;
  uint32_t runtime_offset;
  uint32_t capabilities2;
} __attribute__((packed));

struct XhciOperational {
  uint32_t usb_command;
  uint32_t usb_status;
  uint32_t page_size;
  uint32_t reserved;
  uint32_t reserved2;
  uint32_t device_notification_control;
  uint64_t command_ring_control;
  uint64_t reserved3;
  uint64_t reserved4;
  uint64_t device_context_base;
  uint32_t configure;
} __attribute__((packed));

struct XhciInterrupter {
  uint32_t management;
  uint32_t moderation;
  uint32_t event_ring_segment_table_size;
  uint32_t reserved;
  uint64_t event_ring_segment_table_base_address;
  uint64_t event_ring_dequeue_pointer;
} __attribute__((packed));

struct XhciRuntime {
  uint32_t microframe_index;
  uint32_t reserved1;
  uint64_t reserved2;
  uint64_t reserved3;
  uint64_t reserved4;
  XhciInterrupter interrupters[1024];

} __attribute__((packed));

struct XhciPort {
  uint32_t status_and_control;
  uint32_t power_management;
  uint32_t link_info;
  uint32_t lpm_control;
} __attribute__((packed));

struct XhciSlotContext {
  uint32_t route_speed_entries;
  uint32_t latency_port_number;
  uint32_t parent_and_interrupt;
  uint32_t address_and_state;
  uint64_t reserved1;
  uint64_t reserved2;
} __attribute__((packed));

struct XhciEndpointContext {
  uint32_t state;
  uint32_t error_and_type;
  uint64_t tr_dequeue_ptr;
  uint32_t average_trb_length;
  uint32_t reserved1;
  uint64_t reserved2;
} __attribute__((packed));

struct XhciDeviceContext {
  XhciSlotContext slot_context;
  XhciEndpointContext endpoint_contexts[31];
} __attribute__((packed));

struct XhciTrb {
  uint64_t parameter;
  uint32_t status;
  uint16_t type_and_cycle;
  uint16_t control;
} __attribute__((packed));

struct XhciEventRingSegmentTableEntry {
  uint64_t ring_segment_base;
  uint16_t ring_segment_size;
  uint16_t reserved1;
  uint32_t reserved2;
} __attribute__((packed));
