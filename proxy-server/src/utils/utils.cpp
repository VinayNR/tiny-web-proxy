#include "utils.h"

#include <sstream>

int findPosition(const std::string s, const char *c) {
    for (int i=0; i<s.length(); i++) {
        if (s[i] == *c) {
            return i;
        }
    }
    return -1;
}

void deleteAndNullifyPointer(char *& ptr, bool isArray) {
    isArray ? delete[] ptr : delete ptr;
    ptr = nullptr;
}

int searchKeyValueFromRawString(const std::string & data, const std::string & pattern, char delimiter) {
    size_t pattern_pos;
    std::stringstream ss(data);
    std::string line;
    // extract request line parameters
    while (getline(ss, line, delimiter)) {
        if (line.length() == 0) {
            // end of request
            break;
        }
        if ((pattern_pos = line.find(pattern)) != std::string::npos) {
            return stoi(line.substr(pattern_pos + pattern.length()));
        }
        if (ss.peek() == '\n') {
            ss.ignore();
        }
    }
    return 0;
}

std::string jsonify_cache(Cache<std::string, std::string> * cache) {
    std::stringstream ss;
    ss << "{";

    std::unordered_map<std::string, CacheEntry<std::string>> cache_store = cache->getCacheStore();
    
    for (auto it = cache_store.begin(); it != cache_store.end(); ++it) {
        if (it != cache_store.begin()) {
            ss << ",";
        }

        // Quote keys and convert values to strings
        ss << "\"" << it->first << "\": " << it->second.value;
    }

    ss << "}";
    return ss.str();
}