#
# Please modify only BOOST to match your version
#
# Boost C++ library
BOOST = /Path/To/Boost/boost_1_70_0
CXX := g++
CPPFLAGS = -c -std=c++11
BUILD_DIR := Output

INCLUDE = src


EXEC         = madhat
SOURCES      = $(wildcard src/*.cpp)
OBJECTS      = $(SOURCES:.cpp=.o)
OBJDIR := Output

$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CPPFLAGS)  -I src/stats -I $(INCLUDE)  -I ${BOOST} $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)
