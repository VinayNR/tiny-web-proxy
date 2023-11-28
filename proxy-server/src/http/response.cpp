#include "../http/response.h"
#include "../utils/utils.h"

#include <iostream>
#include <sstream>
#include <string>

HttpResponse::HttpResponse() {
    _http_response_body = nullptr;
    _http_response_body_length = 0;
    serialized_response_ = nullptr;
    serialized_response_length_ = 0;
}

HttpResponse* HttpResponse::setHttpStatusCode(int status_code) {
    _http_status_code = status_code;
    return this;
}

HttpResponse* HttpResponse::setHttpStatusMessage(std::string message) {
    _http_status_message = message;
    return this;
}

HttpResponse* HttpResponse::setHttpVersion(std::string version) {
    _http_version = version;
    return this;
}

HttpResponse* HttpResponse::addResponseHeaders(const std::string & key, const std::string & value) {
    _http_response_headers[key] = value;
    return this;
}

HttpResponse* HttpResponse::setHttpResponseBody(const char * response_body, const int response_length) {
    _http_response_body_length = response_length;

    // dynamically set the response body char pointer
    _http_response_body = new char[response_length + 1];
    memset(_http_response_body, 0, response_length + 1);
    strcpy(_http_response_body, response_body);

    return this;
}

const char * HttpResponse::getHttpResponseBody() {
    return _http_response_body;
}

int HttpResponse::getHttpResponseBodyLength() {
    return _http_response_body_length;
}

const char * HttpResponse::getSerializedResponse() {
    return serialized_response_;
}

int HttpResponse::getSerializedResponseLength() {
    return serialized_response_length_;
}

void HttpResponse::setSerializedResponse(const char * response, int length) {
    serialized_response_ = new char[length + 1];
    memset(serialized_response_, 0, length + 1);
    memcpy(serialized_response_, response, length);
    serialized_response_length_ = length;
}

void HttpResponse::serialize() {
    // the response line and the headers
    std::string response_string_ = "";
    response_string_ += ("HTTP/" + _http_version + " " + std::to_string(_http_status_code) + " " + _http_status_message + "\r\n");
    
    for (const auto &pair: _http_response_headers) {
        response_string_ += (pair.first + ": " + pair.second + "\r\n");
    }

    // the length of entire response
    int response_length = response_string_.size();

    // check if there is a response body to serialize
    if (_http_response_body_length > 0) {
        response_string_ += "\r\n"; // adds a new line

        response_length = response_string_.size() + _http_response_body_length;
    }

    serialized_response_length_ = response_length + 1;
    serialized_response_ = new char[serialized_response_length_];

    memset(serialized_response_, 0, response_length + 1);
    
    strcpy(serialized_response_, response_string_.c_str());

    // set the body
    if (_http_response_body_length > 0) {
        strcat(serialized_response_, _http_response_body);
    }
}

int HttpResponse::extractResponseParams(const std::string & line) {
    std::istringstream first_line(line);
    first_line >> _http_version >> _http_status_code >> _http_status_message;
    return 0;
}

int HttpResponse::extractResponseHeaders(const std::string & line) {
    // assume it's headers (key: value)
    int delim_pos = -1;
    if ((delim_pos = findPosition(line, ":")) == -1) {
        return -1;
    }
    // extract parts of the std::string
    _http_response_headers[line.substr(0, delim_pos)] = line.substr(delim_pos + 2);

    return 0;
}

int HttpResponse::deserialize() {
    std::cout << " ----------- Inside response deserializer ----------- " << std::endl;
    std::stringstream ss(serialized_response_);
    std::string response_line;

    // extract response line parameters
    getline(ss, response_line, '\r');
    if (extractResponseParams(response_line) == -1) {
        perror("Failed to extract request line parameters");
        return -1;
    }
    
    if (ss.peek() == '\n') {
        ss.ignore();
    }

    // extract response headers
    while (getline(ss, response_line, '\r')) {
        if (response_line.length() == 0) {
            break;
        }
        if (extractResponseHeaders(response_line) == -1) {
            perror("Failed to extract request header");
            return -1;
        }
        if (ss.peek() == '\n') {
            ss.ignore();
        }
    }
    if (ss.peek() == '\n') {
        ss.ignore();
    }

    // extract body
    int response_body_length = serialized_response_length_; // just an estimate
    _http_response_body = new char[response_body_length];
    memset(_http_response_body, 0, response_body_length);
    ss.readsome(_http_response_body, response_body_length);

    std::cout << " ----------- Finished response deserializer ----------- " << std::endl;

    return 0;
}

void HttpResponse::displayResponse() {
    std::ostringstream response;
    response << "Response : { HTTP/" << _http_version << ", Status: " << _http_status_code << ", Message: " << _http_status_message << " }" << "\r\n";
    std::cout << response.str();
}

std::string HttpResponse::getHeaderValue(const std::string &header_key) {
    if (_http_response_headers.count(header_key) == 0) {
        return "";
    }
    return _http_response_headers[header_key];
}