#pragma once

#include <stdint.h>

void InitGdt();

void SetIst1(uint64_t* ist1);

void SetRsp0(uint64_t rsp0);
