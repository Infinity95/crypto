#pragma once

#ifndef CRYPTO_UTIL_STRINGS_H
#define CRYPTO_UTIL_STRINGS_H

#include <string>
#include <filesystem>

namespace crypto
{
namespace strings
{

inline std::string filename(std::string filePath)
{
    std::experimental::filesystem::path myPath(filePath);

    return myPath.filename().string();
}

#define __FILENAME__ crypto::strings::filename(__FILE__)

inline std::string removeUntilSubStr(std::string string, const std::string& pattern)
{
    auto pos = string.find(pattern);

    if (pos == std::string::npos)
        return string;

    return string.erase(0, pos);
}

}
}

#endif // CRYPTO_UTIL_STRINGS_H
