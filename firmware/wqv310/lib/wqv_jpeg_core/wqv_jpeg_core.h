#pragma once

#include <ctime>
#include <span>
#include <string>

#include "wqv_types.h"

time_t timestampToTime(const Timestamp src);

std::pair<std::string, Timestamp> getMetaFromJpegMarker(std::span<const uint8_t> data);
