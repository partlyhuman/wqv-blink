#pragma once

#include <Arduino.h>

#include <span>
#include <string>

namespace Image {

void init();
void postProcess(std::string fileName, std::span<const uint8_t> data);

}  // namespace Image