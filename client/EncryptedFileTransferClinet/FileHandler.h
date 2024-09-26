#pragma once

#include <string>
#include <vector>

class FileHandler {
public:
    FileHandler();
    ~FileHandler();

    std::vector<uint8_t> readFile(const std::string& filename);
    void writeFile(const std::string& filename, const std::vector<uint8_t>& data);

private:
    // Add any necessary private members
};