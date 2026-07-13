#pragma once
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <iomanip>

class AHash
{
public:
    static std::string getFileExtension(const std::string& filePath) {
        size_t pos = filePath.find_last_of('.');
        if (pos == std::string::npos) {
            return "";
        }
        return filePath.substr(pos);
    }

    static std::string make(const std::string& filename) {

        const uint64_t FNV_prime = 1099511628211u;
        const uint64_t offset_basis = 14695981039346656037u;

        uint64_t hash = offset_basis;
        for (char c : filename) {
            hash ^= c;
            hash *= FNV_prime;
        }

        std::stringstream ss;
        ss << std::hex << std::setw(16) << std::setfill('0') << hash;

        std::string newFile = ss.str();
        std::string extension = getFileExtension(filename);
        return newFile + extension;
    };
};