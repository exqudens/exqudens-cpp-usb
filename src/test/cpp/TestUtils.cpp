#include <cstddef>
#include <cctype>
#include <stdexcept>
#include <filesystem>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <fstream>

#include "TestUtils.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

std::vector<std::string> TestUtils::toStringVector(
        const std::exception& exception,
        std::vector<std::string> previous
) {
    previous.emplace_back(exception.what());
    try {
        std::rethrow_if_nested(exception);
        return previous;
    } catch (const std::exception& e) {
        return toStringVector(e, previous);
    } catch (...) {
        if (previous.empty()) {
            previous.emplace_back(CALL_INFO + ": Empty stack!");
        }
        return previous;
    }
}

std::vector<std::string> TestUtils::toStackTrace(const std::exception& exception) {
    try {
        std::vector<std::string> elements = toStringVector(exception);
        if (elements.size() > 1) {
            std::ranges::reverse(elements);
        }
        return elements;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::toString(const std::exception& exception) {
    try {
        std::vector<std::string> stackTrace = toStackTrace(exception);
        std::ostringstream out;
        for (size_t i = 0; i < stackTrace.size(); i++) {
            out << stackTrace[i];
            if (i < stackTrace.size() - 1) {
                out << std::endl;
            }
        }
        return out.str();
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::ltrim(const std::string& value) {
    try {
        std::string str = value;
        str.erase(
                str.begin(),
                std::find_if(str.begin(), str.end(), [](char c) {return !std::isspace(c);})
        );
        return str;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::rtrim(const std::string& value) {
    try {
        std::string str = value;
        str.erase(
                std::find_if(str.rbegin(), str.rend(), [](char c) {return !std::isspace(c);}).base(),
                str.end()
        );
        return str;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::trim(const std::string& value) {
    try {
        std::string str = value;
        str = ltrim(str);
        str = rtrim(str);
        return str;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::toUpper(const std::string& value) {
    try {
        std::string str = value;
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::toLower(const std::string& value) {
    try {
        std::string str = value;
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::replaceAll(const std::string& value, const std::string& target, const std::string& replacement) {
    try {
        std::string str = value;
        if (target == replacement) {
            return str;
        }
        std::size_t foundAt = std::string::npos;
        while ((foundAt = str.find(target, foundAt + 1)) != std::string::npos) {
            str.replace(foundAt, target.length(), replacement);
        }
        return str;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::vector<char> TestUtils::readFileBytes(const std::string& path) {
    try {
        std::filesystem::path filePath = std::filesystem::path(path).make_preferred();
        std::size_t fileSizeRaw = std::filesystem::file_size(filePath);

        if (fileSizeRaw > LLONG_MAX) {
            throw std::runtime_error(CALL_INFO + ": File size is larger than read max size: '" + std::to_string(fileSizeRaw) + " > " + std::to_string(LLONG_MAX) + "'!");
        }

        auto fileSize = (std::streamsize) fileSizeRaw;
        std::vector<char> buffer(fileSize);
        std::ifstream inputStream(filePath.string(), std::ios::binary);

        if (!inputStream.is_open()) {
            throw std::runtime_error(CALL_INFO + ": Failed to open file: '" + path + "'!");
        }

        inputStream.read(buffer.data(), fileSize);
        inputStream.close();

        return buffer;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::readFileString(const std::string& path) {
    try {
        std::vector<char> bytes = readFileBytes(path);
        std::string result(bytes.begin(), bytes.end());

        return result;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::vector<std::string> TestUtils::split(const std::string& value, const std::string& delimiter) {
    try {
        std::vector<std::string> result = {};

        if (value.empty()) {
            return result;
        }

        if (delimiter.empty()) {
            for (size_t i = 0; i < value.size(); i++) {
                std::string token = value.substr(i, 1);
                result.emplace_back(token);
            }
        } else {
            std::string s = value;
            size_t p = 0;
            std::string token = "";
            while ((p = s.find(delimiter)) != std::string::npos) {
                token = s.substr(0, p);
                result.emplace_back(token);
                s.erase(0, p + delimiter.size());
            }
            token = s.substr(0, p);
            result.emplace_back(token);
        }

        return result;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::join(const std::vector<std::string>& value, const std::string& delimiter, const std::string& prefix, const std::string& suffix) {
    try {
        std::string result;
        if (value.empty()) {
            return result;
        }
        result = std::accumulate(
                value.begin(),
                value.end(),
                std::string(),
                [&delimiter](const std::string& a, const std::string& b) {
                    return a + (a.empty() ? "" : delimiter) + b;
                }
        );
        result = prefix + result + suffix;
        return result;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

#undef CALL_INFO
