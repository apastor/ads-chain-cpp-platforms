CXX=g++

CPPFLAGS=-std=gnu++17 -rdynamic
LDLIBS=-lcppcms -lbooster -ladschain -lfruit -lPocoUtil


all: ttj
	
ttj: ttj.cpp
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDLIBS)

