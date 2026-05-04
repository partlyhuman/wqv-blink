#include "image.h"

#include <FFat.h>

#include <cstring>
#include <ctime>
#include <unordered_map>

#include "log.h"
#include "types.h"

namespace Image {

const static char *TAG = "Image";
static std::unordered_map<std::string, size_t> timestampCounts;

void init() {
    timestampCounts.clear();
}

time_t timestampToTime(const Timestamp src) {
    tm t{};
    t.tm_year = (2000 + src.year2k) - 1900;
    t.tm_mon = src.month - 1;
    t.tm_mday = src.day;
    t.tm_hour = src.hour;
    t.tm_min = src.minute;
    t.tm_isdst = 0;
    time_t time = mktime(&t);
    return time;
}

void setSystemTime(const time_t time) {
    timeval epoch = {time, 0};
    settimeofday((const timeval *)&epoch, 0);
}

std::string trimTrailingSpaces(std::string src) {
    if (src.find_first_not_of(' ') == std::string::npos) return "";
    return src.substr(0, src.find_last_not_of(' ') + 1);
}

std::pair<std::string, Timestamp> getMetaFromJpegMarker(std::span<const uint8_t> header) {
    try {
        // Easier to use as char here
        auto iter = header.begin();

        if (iter[0] != 0xff || iter[1] != 0xd8) throw std::runtime_error("Invalid SOI");
        iter += 2;

        // Make sure we at least have enough to read the marker
        while ((iter + 4) < header.end()) {
            if (iter[0] != 0xff) throw std::runtime_error("Expected 0xff in marker");
            uint8_t markerId = iter[1];
            iter += 2;

            // len includes the bytes themselves, but we don't want to consume them
            uint16_t len = (iter[0] << 8) | iter[1];
            LOGV(TAG, "Encountered marker %02x of length %d", marker, len);
            if (markerId != 0xe2) {
                iter += len;
                continue;
            }

            LOGD(TAG, "Found APP2 marker, extracting metadata");
            std::span<const uint8_t> marker(iter + 2, iter + len);
            if (marker.size() < sizeof(Timestamp)) throw std::runtime_error("APP2 marker not big enough for timestamp");

            std::string title{};
            bool isAscii =
                std::all_of(marker.begin(), marker.end() - sizeof(Timestamp), [](char b) { return b <= 0x7F; });
            if (isAscii) {
                title = trimTrailingSpaces(
                    std::string(reinterpret_cast<const char *>(marker.data()), marker.size() - sizeof(Timestamp)));
            }

            Timestamp timestamp;
            std::memcpy(&timestamp, marker.data() + marker.size() - sizeof(Timestamp), sizeof(Timestamp));

            return {title, timestamp};
        }
        LOGE(TAG, "Couldn't find APP2 marker in JPEG, no metadata");
    } catch (std::exception &e) {
        LOGE(TAG, "%s", e.what());
    }
    return {};
}

bool saveToFlash(std::span<const uint8_t> data, std::string destPath) {
    auto startTime = millis();
    File dst = FFat.open(destPath.c_str(), "w");
    if (!dst) {
        LOGE(TAG, "Couldn't open file for writing: %s", destPath.c_str());
        return false;
    }
    dst.write(data.data(), data.size());
    dst.close();
    LOGD(TAG, "Copied file in %d ms", millis() - startTime);
    return true;
}

static inline std::string pad2(int v) {
    return v < 10 ? "0" + std::to_string(v) : std::to_string(v);
}

std::string getBaseFilename(const Timestamp t) {
    std::string basepath;
    // File naming strategies:
    // - count: simple, short filenames, will always collide with multiple syncs. could persist last count
    // static int count = 0;
    // basepath = pad2(++count);
    // - including whole date and time, plus count: legible, no collisions, but long
    // basepath = std::to_string(img->month) + "-" + std::to_string(img->day) + "-" +
    //            std::to_string(2000 + img->year_minus_2000) + "_" + pad2(img->hour) + "-" + pad2(img->minute);
    // - just YYYYMMDD, plus count
    basepath = std::to_string(t.year2k + 2000) + pad2(t.month) + pad2(t.day);
    // - unix style timestamp, plus count: shorter, no collisions, not very legible
    // basepath = std::to_string(time);

    int count = timestampCounts[basepath]++;
    return "WQV" + basepath + "_" + pad2(count + 1);
}

void postProcess(std::string fileName, std::span<const uint8_t> data) {
    std::string dir = "/";

    auto [title, timestamp] = getMetaFromJpegMarker(data);

    std::string base = getBaseFilename(timestamp);
    time_t time = timestampToTime(timestamp);
    setSystemTime(time);

    LOGI(TAG, "File %s has metadata time=%s title='%s'", fileName.c_str(), std::ctime(&time), title.c_str());

    // Write out the title into a sidecar file if it exists
    if (title.size() > 0) {
        auto file = FFat.open((dir + base + "_title.txt").c_str(), "w", true);
        if (file) {
            file.println(title.c_str());
            file.close();
        }
    }

    saveToFlash(data, dir + base + ".jpg");
}

}  // namespace Image