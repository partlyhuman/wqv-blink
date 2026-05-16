#pragma once

#include <cstring>
#include <span>
#include <string>

namespace Image {

void init();
void postProcess(std::string fileName, std::vector<uint8_t> data);

}  // namespace Image