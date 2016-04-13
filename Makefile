CXX = g++-5
# CXXFLAGS    = -std=c++11 -g -Wall -O3 -funroll-loops -mavx -mtune=native
CXXFLAGS    = -std=c++11 -g -Wall -mavx -mtune=native
# -Winline -fcilkplus 
# -Wall -Wno-missing-braces -Wno-unused-variable  -Wno-attributes

TARGET = cycle
all: $(TARGET)

cycle.o: cycle.cpp perm16.hpp
cycle: cycle.o perm16.o
	$(CXX) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@
