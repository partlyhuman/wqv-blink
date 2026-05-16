#include <unity.h>

#include <filesystem>
#include <fstream>
#include <span>
#include <string>
#include <vector>

#include "wqv_jpeg_core.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

std::vector<uint8_t> load(const std::string& path) {
    std::ifstream file(path, std::ios::binary);

    TEST_ASSERT_TRUE(file.is_open());

    file.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    file.close();

    return data;
}

void save(std::span<const uint8_t> data, const std::string& path) {
    std::ofstream file(path, std::ios::binary);

    TEST_ASSERT_TRUE(file.is_open());

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    file.close();
}

void test_load_file() {
    const std::string path = std::filesystem::path(__FILE__).parent_path() / "jpeg_with_title.jpg";
    auto data = load(path);
    TEST_ASSERT_EQUAL(4204, data.size());
}

void test_parse_title() {
    const std::string path = std::filesystem::path(__FILE__).parent_path() / "jpeg_with_title.jpg";
    auto data = load(path);
    auto [title, timestamp] = parseCasioJpegMetadata(data);

    TEST_ASSERT_EQUAL_STRING("MR  RA~~    IN GASTO", title.c_str());
}

void test_parse_time() {
    const std::string path = std::filesystem::path(__FILE__).parent_path() / "known_time.jpg";
    auto data = load(path);
    auto [title, timestamp] = parseCasioJpegMetadata(data);

    // I shot this 5/16 14:36 2026
    TEST_ASSERT_EQUAL(26, timestamp.year2k);
    TEST_ASSERT_EQUAL(5, timestamp.month);
    TEST_ASSERT_EQUAL(16, timestamp.day);
    TEST_ASSERT_EQUAL(14, timestamp.hour);
    TEST_ASSERT_EQUAL(36, timestamp.minute);
}

void test_erase_casio_jpeg_tags() {
    const std::string path = std::filesystem::path(__FILE__).parent_path() / "jpeg_with_title.jpg";
    auto data = load(path);
    parseCasioJpegMetadata(data, true);
    save(data, std::filesystem::path(__FILE__).parent_path() / "jpeg_with_title_stripped.jpg");
}

void test_make_exif() {
    const std::string path = std::filesystem::path(__FILE__).parent_path() / "jpeg_with_title.jpg";
    auto data = load(path);
    auto [title, timestamp] = parseCasioJpegMetadata(data);

    auto exif = makeExifBlob(timestamp, title, 10);
    TEST_ASSERT_GREATER_THAN(0, exif.size());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_load_file);
    RUN_TEST(test_parse_title);
    RUN_TEST(test_parse_time);
    RUN_TEST(test_erase_casio_jpeg_tags);
    RUN_TEST(test_make_exif);
    return UNITY_END();
}
