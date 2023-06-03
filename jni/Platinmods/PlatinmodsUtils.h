//
// Created by Mika Cybertron on 5/20/2023.
//
#pragma once

#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace PlatinmodsUtils {

    std::string Hex2String(const char* buffer, size_t size);
    void setFormatHex(std::string &hexCode);
    void trim_string(std::string &str);
    bool validateHexString(std::string &xstr);
    void toHex(void *const data, const size_t dataLength, std::string &dest);
    void fromHex(const std::string &in, void *const data);
}
