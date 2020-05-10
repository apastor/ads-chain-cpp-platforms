CXX=g++

CPPFLAGS=-std=gnu++17 -rdynamic
LDLIBS=-lcppcms -lbooster -ladschain -lfruit -lpthread -lPocoUtil


all: dsp
	
dsp: dsp.cpp
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDLIBS)

