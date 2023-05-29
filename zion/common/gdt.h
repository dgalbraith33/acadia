#pragma once

#include <stdint.h>

void InitGdt();

void SetRsp0(uint64_t rsp0);
