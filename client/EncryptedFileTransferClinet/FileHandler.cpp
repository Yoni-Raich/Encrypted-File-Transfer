#include "FileHandler.h"
#include <fstream>
#include <stdexcept>

FileHandler::FileHandler() {
}

FileHandler::~FileHandler() {
}

std::vector<uint8_t> FileHandler::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unable to open file for reading");
    }

    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
}

void FileHandler::writeFile(const std::string& filename, const std::vector<uint8_t>& data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unable to open file for writing");
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}