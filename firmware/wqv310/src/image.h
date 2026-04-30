#pragma once

#include <Arduino.h>

#include <span>
#include <string>
#include <utility>

namespace Image {

void init();
void postProcess(std::string fileName, const std::vector<uint8_t> &buffer);

}  // namespace Image