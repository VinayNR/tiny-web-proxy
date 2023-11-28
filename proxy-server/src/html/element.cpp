#include "element.h"

std::string HtmlElement::getTagName() const {
    return tag_name_;
}

void HtmlElement::setTagName(std::string tag_name) {
    tag_name_ = tag_name;
}

void HtmlElement::addElementProperty(std::string key, std::string value) {
    element_props_[key] = value;
}

std::string HtmlElement::getElementPropertyByKey(std::string key) {
    if (element_props_.count(key) == 0) {
        return "";
    }
    return element_props_[key];
}