#pragma once

#include <Arduino.h>

namespace Firmware
{
void init();
void rebootIntoNextOtaPartition();
void rebootIntoOtaPartition(uint part);
}