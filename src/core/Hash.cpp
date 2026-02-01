#include <array>
#include <fstream>
#include <vector>
#include <filesystem>

std::array<uint8_t, 32> simple_hash(const std::vector<uint8_t>& data) {
    std::array<uint8_t, 32> hash{};
    uint32_t acc = 0xDEADBEEF;

    for (auto b : data)
        acc = (acc ^ b) * 16777619u;

    for (int i = 0; i < 32; ++i)
        hash[i] = (acc >> ((i % 4) * 8)) & 0xFF;

    return hash;
}

std::array<uint8_t, 32> hash_file(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    const std::vector<uint8_t> data(
        std::istreambuf_iterator<char>(file),
        {}
    );
    return simple_hash(data);
}
