#!/bin/sh
cd PCRE2Plus
g++ -std=c++11 -D_UNICODE=1 `pcre2-config --cflags` example.cpp PCRE2plus.cpp `pcre2-config --libs16 --libs32 --libs8` -o ../pcre.run
