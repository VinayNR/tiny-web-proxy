#pragma once

#include <string>
#include <unordered_map>

#include "../proxy/cache.h"

int findPosition(const std::string, const char *);

void deleteAndNullifyPointer(char *&, bool);

int searchKeyValueFromRawString(const std::string &, const std::string &, char);

std::string jsonify_cache(Cache<std::string, std::string> *);