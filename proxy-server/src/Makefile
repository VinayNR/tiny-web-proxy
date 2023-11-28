CC = g++  # Compiler
CFLAGS = -std=c++11  # Compiler flags

proxy = proxy/*.cpp
http = http/*.cpp
html = html/*.cpp
utils = utils/*.cpp

all:
	$(CC) $(CFLAGS) $(proxy) $(http) $(html) $(utils) -o server

clean:
	rm -f $(server)

.PHONY: all clean