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

    return data;
}

void test_load_file() {
    const std::string path = std::filesystem::path(__FILE__).parent_path() / "jpeg_with_title.jpg";
    auto data = load(path);
    TEST_ASSERT_EQUAL(4204, data.size());
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_load_file);

    return UNITY_END();
}
