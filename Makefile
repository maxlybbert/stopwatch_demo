.POSIX: ;
SHELL=/bin/sh
CXX?=clang++
CXXFLAGS+=-std=c++11 -W -Wall -O3 -I. -g
CXXFLAGS+=`pkg-config gtkmm-3.0 --cflags`
LDFLAGS+=`pkg-config gtkmm-3.0 --libs`
OBJS:=main.o stopwatch.o

build: $(OBJS)
	$(CXX) $(LDFLAGS) -o stopwatch $(OBJS)

.cc:
	$(CXX) -c $< $(CXXFLAGS) -o $@

clean:
	-rm stopwatch $(OBJS)

all: build ;

