#pragma once

#include <stdint.h>

template <typename T>
class RequestConstants {
 public:
  static uint8_t RequestType();
  static uint8_t Request();
  static uint16_t Value();
};

struct DeviceDescriptor {
  uint8_t length;
  uint8_t type;
  uint16_t usb_spec;
  uint8_t device_class;
  uint8_t device_subclass;
  uint8_t device_protocol;
  uint8_t max_packet_size_exp;
  uint16_t vendor_id;
  uint16_t product_id;
  uint16_t device_release;
  uint8_t manufacturer_string_index;
  uint8_t product_string_index;
  uint8_t serial_string_index;
  uint8_t num_configurations;
} __attribute__((packed));

template <>
class RequestConstants<DeviceDescriptor> {
 public:
  static uint8_t RequestType() { return 0x80; }
  static uint8_t Request() { return 0x6; };
  static uint16_t Value() { return 0x100; };
};

struct ConfigurationDescriptor {
  uint8_t length;
  uint8_t type;
  uint16_t total_length;
  uint8_t num_interfaces;
  uint8_t configuration_value;
  uint8_t configuration_string_index;
  uint8_t attributes;
  uint8_t max_power;
} __attribute__((packed));

template <>
class RequestConstants<ConfigurationDescriptor> {
 public:
  static uint8_t RequestType() { return 0x80; }
  static uint8_t Request() { return 0x6; }
  // TODO: Consider getting other description indexes.
  static uint16_t Value() { return 0x200; }
};

struct InterfaceDescriptor {
  uint8_t length;
  uint8_t type;
  uint8_t interface_number;
  uint8_t alternate_setting;
  uint8_t num_endpoints;
  uint8_t interface_class;
  uint8_t interface_subclass;
  uint8_t interface_protocol;
  uint8_t interfact_string_index;
} __attribute__((packed));

struct EndpointDescriptor {
  uint8_t length;
  uint8_t type;
  uint8_t endpoint_address;
  uint8_t attributes;
  uint16_t max_packet_size;
  uint8_t interval;
} __attribute__((packed));
