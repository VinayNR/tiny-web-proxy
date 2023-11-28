#pragma once

#include <string>
#include <vector>


class HtmlElement {
    private:
        std::string tag_name_;
        
        // property
        std::unordered_map<std::string, std::string> element_props_;

        std::vector<HtmlElement *> children;
        HtmlElement* parent;
        std::string body;

    public:
        std::string getTagName() const;
        void setTagName(std::string);

        void addElementProperty(std::string, std::string);
        std::string getElementPropertyByKey(std::string);

};