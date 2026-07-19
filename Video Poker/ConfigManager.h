#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>

class ConfigManager {
private:
    std::unordered_map<std::string, std::string> configData;

    // Helper function to trim whitespace from both ends of a string
    std::string trim(const std::string& str) const {
        size_t first = str.find_first_not_of(' ');
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    // Helper function to convert string to lowercase
    std::string toLower(const std::string& str) const {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    // Helper function to determine if a string represents a boolean
    bool isBoolean(const std::string& str) const {
        std::string lowerStr = toLower(trim(str));
        return (lowerStr == "true" || lowerStr == "false" ||
            lowerStr == "1" || lowerStr == "0");
    }

    // Helper function to determine if a string represents an integer
    bool isInteger(const std::string& str) const {
        if (str.empty()) return false;
        size_t start = 0;
        if (str[0] == '-' || str[0] == '+') {
            if (str.length() == 1) return false;
            start = 1;
        }
        for (size_t i = start; i < str.length(); ++i) {
            if (!std::isdigit(str[i])) return false;
        }
        return true;
    }

    // Helper function to determine if a string represents a decimal
    bool isDecimal(const std::string& str) const {
        if (str.empty()) return false;
        size_t dotCount = 0;
        size_t start = 0;
        if (str[0] == '-' || str[0] == '+') {
            if (str.length() == 1) return false;
            start = 1;
        }
        for (size_t i = start; i < str.length(); ++i) {
            if (str[i] == '.') {
                dotCount++;
                if (dotCount > 1) return false;
            }
            else if (!std::isdigit(str[i])) {
                return false;
            }
        }
        return dotCount == 1;
    }

public:
    // Constructor that loads configuration from file
    ConfigManager(const std::string& filename) {
        loadConfig(filename);
    }

    // Load configuration from file
    void loadConfig(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open configuration file: " + filename);
        }

        std::string line;
        int lineNumber = 0;

        while (std::getline(file, line)) {
            lineNumber++;

            // Skip empty lines and comments (lines starting with #)
            if (line.empty() || line[0] == '#') {
                continue;
            }

            // Find the first '=' character
            size_t delimiterPos = line.find('=');
            if (delimiterPos == std::string::npos) {
                std::cerr << "Warning: Invalid line format at line " << lineNumber
                    << ": " << line << std::endl;
                continue;
            }

            // Extract key and value
            std::string key = trim(line.substr(0, delimiterPos));
            std::string value = trim(line.substr(delimiterPos + 1));

            // Remove quotes from string values if present
            if (value.length() >= 2 &&
                ((value.front() == '"' && value.back() == '"') ||
                    (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.length() - 2);
            }

            if (!key.empty()) {
                configData[key] = value;
            }
        }

        file.close();
    }

    // Get value as string
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        auto it = configData.find(key);
        if (it != configData.end()) {
            return it->second;
        }
        return defaultValue;
    }

    // Get value as integer
    int getInt(const std::string& key, int defaultValue = 0) const {
        auto it = configData.find(key);
        if (it != configData.end()) {
            try {
                return std::stoi(it->second);
            }
            catch (const std::exception&) {
                std::cerr << "Warning: Could not convert '" << it->second
                    << "' to integer for key '" << key << "'" << std::endl;
            }
        }
        return defaultValue;
    }

    // Get value as double
    double getDouble(const std::string& key, double defaultValue = 0.0) const {
        auto it = configData.find(key);
        if (it != configData.end()) {
            try {
                return std::stod(it->second);
            }
            catch (const std::exception&) {
                std::cerr << "Warning: Could not convert '" << it->second
                    << "' to double for key '" << key << "'" << std::endl;
            }
        }
        return defaultValue;
    }

    // Get value as boolean
    bool getBool(const std::string& key, bool defaultValue = false) const {
        auto it = configData.find(key);
        if (it != configData.end()) {
            std::string lowerValue = toLower(it->second);
            if (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" || lowerValue == "on") {
                return true;
            }
            else if (lowerValue == "false" || lowerValue == "0" || lowerValue == "no" || lowerValue == "off") {
                return false;
            }
            else {
                std::cerr << "Warning: Could not convert '" << it->second
                    << "' to boolean for key '" << key << "'" << std::endl;
            }
        }
        return defaultValue;
    }

    // Check if a key exists
    bool hasKey(const std::string& key) const {
        return configData.find(key) != configData.end();
    }

    // Get all keys
    std::vector<std::string> getAllKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : configData) {
            keys.push_back(pair.first);
        }
        return keys;
    }

    // Print all configuration data
    void printAll() const {
        for (const auto& pair : configData) {
            std::cout << pair.first << " = " << pair.second << std::endl;
        }
    }

    // Get the type of a value (for informational purposes)
    std::string getValueType(const std::string& key) const {
        auto it = configData.find(key);
        if (it == configData.end()) {
            return "undefined";
        }

        const std::string& value = it->second;

        if (isBoolean(value)) {
            return "boolean";
        }
        else if (isInteger(value)) {
            return "integer";
        }
        else if (isDecimal(value)) {
            return "decimal";
        }
        else {
            return "string";
        }
    }
};

#endif