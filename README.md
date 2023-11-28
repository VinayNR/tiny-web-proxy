# Tiny Web Proxy Server
The Tiny Web Proxy Server is a lightweight proxy server written in C++ that serves as an intermediary between a web client and a web server. Its primary responsibilities include relaying requests from the client to the server and vice versa.

## Features
### Http Request Relay
The main purpose of the proxy server is to listen for client requests on any given port, capturing HTTP requests (GET and POST). It then relays these requests to the HTTP server on behalf of the client.

### Multithreading
The proxy server utilizes a pool of threads to handle incoming client connections. The default thread count is 20, but this is configurable. The main proxy thread listens for incoming connect requests, updating a common queue of requests with the file descriptor ID. Worker threads passively listen for updates to the queue, grabbing work when notified. This approach prevents individual threads from monopolizing CPU cycles on the server.

### Caching
To enhance performance, the proxy server maintains caches for storing responses from the server and caching DNS query responses. The cache is designed to handle binary data (e.g., png, pdf) and is implemented as a key-value store of strings and vector<char>. On subsequent requests, the server checks its cache for the entry and returns it to the client if present and not expired. Each cache is thread-safe for synchronous access across all threads.

### Link Prefetching
The proxy server parses HTML responses from any web server, identifying links that can be prefetched by examining link and anchor tags in the HTML. Each of these links is then requested from the server ahead of time, and the responses are stored in the cache.

### Denylist
For security and control, the proxy server maintains a denylist of blocked websites. When a client attempts to access these blocked domains, the server produces a 403 Forbidden error message, denying access.

## Usage
Clone the project in a directory on the system, and run the makefile command
`make`
