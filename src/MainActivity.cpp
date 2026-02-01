#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <string>
#include "model/FileInfo.h"

namespace fs = std::filesystem;

std::unordered_map<std::string, FileInfo> scan_files(const fs::path& root) {
    std::unordered_map<std::string, FileInfo> result;

    for (const auto& entry : fs::recursive_directory_iterator(root)) {
        if (!entry.is_regular_file())
            continue;

        try {
            auto size = entry.file_size();
            result[entry.path().string()] = { size };
        } catch (...) {
            // dont care
        }
    }

    return result;
}

void save_baseline(
    const std::unordered_map<std::string, FileInfo>& files,
    const std::string& file
) {
    std::ofstream out(file);
    if (!out)
        throw std::runtime_error("Cannot create baseline file");

    for (const auto& [path, info] : files) {
        out << path << " | " << info.size << "\n";
    }

    out.close();
}

std::unordered_map<std::string, FileInfo> load_baseline(const std::string& file) {
    std::unordered_map<std::string, FileInfo> result;
    std::ifstream in(file);

    if (!in)
        throw std::runtime_error("Baseline missing or corrupted");

    std::string line;
    while (std::getline(in, line)) {
        const auto sep = line.find('|');

        if (sep == std::string::npos)
            continue;

        std::string path = line.substr(0, sep);
        const uintmax_t size = std::stoull(line.substr(sep + 1));

        result[path] = { size };
    }

    in.close();

    return result;
}

void verify(
    const std::unordered_map<std::string, FileInfo>& baseline,
    const std::unordered_map<std::string, FileInfo>& current
) {
    for (const auto& [path, info] : baseline) {
        auto it = current.find(path);

        if (it == current.end()) {
            std::cout << "[MISSING] " << path << "\n";
        } else if (it->second.size != info.size) {
            std::cout << "[MODIFIED] " << path << "\n";
        }
    }
}

int main() {
    const std::string BASELINE_FILE = "baseline.txt";
    const fs::path SCAN_DIR = fs::current_path();

    try {
        if (!fs::exists(BASELINE_FILE)) {
            std::cout << "[WAIT] Creating baseline...\n";
            auto snap = scan_files(SCAN_DIR);
            save_baseline(snap, BASELINE_FILE);
            std::cout << "[+] Baseline created. Restart program.\n";
            return 0;
        }

        std::cout << "[ANTI-CHEAT] Verifying integrity...\n";
        auto baseline = load_baseline(BASELINE_FILE);
        auto current = scan_files(SCAN_DIR);

        verify(baseline, current);

        std::cout << "[DONE]\n";
    }
    catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
