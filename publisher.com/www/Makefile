CXX=g++

CPPFLAGS=-std=gnu++17 -rdynamic
LDLIBS=-lcppcms -lbooster -ladschain -lfruit -lPocoUtil

APPNAME=mostachio
vpath %.tmpl templates

all: $(APPNAME)

$(APPNAME): $(addsuffix .cpp,$(APPNAME)) my_skin
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDLIBS)

%: %.cpp
	$(CXX) $(CPPFLAGS) -shared -fPIC my_skin.cpp -o libmy_skin.so -lcppcms -lbooster

my_skin.cpp: index.tmpl
	cd templates && find $(pwd) -type f -name '*.tmpl' \
	  -not -name 'master.tmpl' -exec \
	  cppcms_tmpl_cc -o ../my_skin.cpp master.tmpl {} +

