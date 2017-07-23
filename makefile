## I am a comment, and I want to say that the variable CC will be
# the compiler to use.
CXX = clang++
SDL = -framework SDL2 -framework SDL2_image
# If your compiler is a bit older you may need to change -std=c++11 to -std=c++0x
CXXFLAGS = -Wall -c -std=c++11
LDFLAGS = $(SDL)
EXE = Ember

all: $(EXE)

$(EXE): main.o intel.o tileRender.o Units.o
	$(CXX) $(LDFLAGS) main.o intel.o tileRender.o Units.o -o Ember

main.o: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp

intel.o: intel.cpp
	$(CXX) $(CXXFLAGS) intel.cpp

tileRender.o: tileRender.cpp
	$(CXX) $(CXXFLAGS) tileRender.cpp

Unit.o: Units.cpp
	$(CXX) $(CXXFLAGS) Units.cpp

clean:
	rm *.o && rm $(EXE)
