#include "parser.h"

#include <regex>
#include <iostream>

/*
* Parses a plain HTML string into a tree structure DOM
*/
HtmlElement* HtmlParser::parseHtmlString(std::string html_string) {
    // int curr_pos = 0, c_end = html_string.length() - 1;

    // HtmlParsingState curr_state;
    
    // while (curr_pos < c_end) {
    //     char curr_char = html_string[curr_pos];

    //     if (curr_char == '<') {

    //     }

    //     curr_pos ++;
    // }
    return nullptr;
}

// default constructor
HtmlParser::HtmlParser() {
}

HtmlParser * HtmlParser::getInstance() {
    static HtmlParser instance;
    return &instance;
}

std::vector<UriElement> HtmlParser::getPatterns(const std::string & html_string, std::string tag_name, std::regex pattern) {
    std::vector<UriElement> matched_elements;
    std::sregex_iterator it(html_string.begin(), html_string.end(), pattern);
    std::sregex_iterator end;

    // Loop through matches and print the matched strings
    while (it != end) {
        std::smatch match = *it;
        matched_elements.push_back(UriElement{tag_name, match[1]});
        ++it;
    }
    
    return matched_elements;
}

/*
* Gets all URI elements within a HTML string
* Returns the list of fetchable links found in the HTML string
* Looks at a, link and img tags
*/
std::vector<UriElement> HtmlParser::getAllUriElements(HttpResponse * http_response) {
    std::vector<UriElement> all_uri_elements;

    // Find all anchor tags
    std::regex anchor_pattern(R"(<a\s.*?href=["']([^"']*)["'][^>]*>(.*?)</a>)");
    std::string a_tag("a");
    std::vector<UriElement> res = getPatterns(http_response->getSerializedResponse(), a_tag, anchor_pattern);
    all_uri_elements.insert(all_uri_elements.end(), res.begin(), res.end());

    // Find all link tags
    std::regex link_pattern(R"(<link\s.*?\brel=["'][^"']*["']\s.*?\bhref=["']([^"']*)["'].*?>)");
    std::string link_tag("link");
    res = getPatterns(http_response->getSerializedResponse(), link_tag, link_pattern);
    all_uri_elements.insert(all_uri_elements.end(), res.begin(), res.end());
    
    return all_uri_elements;
}