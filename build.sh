#!/bin/sh
cd src
g++ -std=c++11 -D_UNICODE=1 `pcre2-config --cflags` ../Example/example.cpp PCRE2plus.cpp `pcre2-config --libs16 --libs32 --libs8` -o ../pcre.run
