CXX = g++
CXXFLAGS = -Wall -Werror -Wpedantic -std=c++11
LIBS =

bmee: src/main.cpp bmee.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: src/%.cpp src/%.hpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(LIBS)

clean:
	rm -rf *.o bmee
