# ImageServer
HTTP server which takes image and mirrors it, then return it.

This app was developed on MacOs with usage of GCC compiler. 

Used additional libraries:
1) Magick++ to work witth jpeg
2) libevent to work with http requests

To compile this app use this command:
g++ ``Magick++-config --cxxflags --cppflags`` -o server server.cpp -std=c++17  ``Magick++-config --ldflags --libs`` -levent

You also need to download libs by yourself
