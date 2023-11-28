#include "../html/parser.h"

#include <iostream>

int main() {
    HtmlParser parser;

    std::string htmlWithLinks = R"(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>HTML with Links</title>
        </head>
        <body>
            <h1>Sample HTML with Links</h1>
            <p>This is a paragraph with a link: <a href="https://www.example.com">Example Website</a></p>
            <p>Another link: <a href="https://www.google.com">Google</a></p>
        </body>
        </html>
    )";

    std::string htmlWithLinksAndAnchors = R"(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>HTML with Links and Anchor Links</title>
            <link rel="stylesheet" href="styles.css">
        </head>
        <body>
            <h1>Sample HTML with Links and Anchor Links</h1>
            <p>This is a paragraph with a link: <a href="https://www.example.com">Example Website</a></p>
            <p>Another link: <a href="https://www.google.com">Google</a></p>
            <ul>
                <li><a href="#section1">Section 1</a></li>
                <li><a href="#section2">Section 2</a></li>
                <li><a href="#section3">Section 3</a></li>
            </ul>
            <section id="section1">
                <h2>Section 1</h2>
                <p>Content for Section 1.</p>
            </section>
            <section id="section2">
                <h2>Section 2</h2>
                <p>Content for Section 2.</p>
            </section>
            <section id="section3">
                <h2>Section 3</h2>
                <p>Content for Section 3.</p>
            </section>
        </body>
        </html>
    )";

    std::vector<UriElements> elements = parser.getAllUriElements(htmlWithLinksAndAnchors);
    for (auto element: elements) {
        std::cout << element.tag_name << ", " << element.uri << std::endl;
    }
}