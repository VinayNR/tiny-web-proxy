#pragma once

#include <string>
#include <vector>
#include <regex>

#include "element.h"
#include "../http/response.h"

enum HtmlParsingState {
    BEGIN,
    START_TAG,
    END_TAG
};

struct UriElement {
    std::string tag_name;
    std::string uri;
};

class HtmlParser {
    private:
        std::vector<UriElement> getPatterns(const std::string &, std::string, std::regex);

        // private constructor
        HtmlParser();

        // Private copy constructor and copy assignment operator to prevent cloning
        HtmlParser(const HtmlParser&) = delete;
        HtmlParser& operator=(const HtmlParser&) = delete;

    public:
        static HtmlParser * getInstance();

        HtmlElement* parseHtmlString(std::string);

        std::vector<UriElement> getAllUriElements(HttpResponse *);
};