# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17
LDFLAGS = -L./ncmlib -lncmlib -lssl -lcrypto

# Include directories
INCLUDES = -I./ncmpp/include -I./ncmlib/include

# Source files
SOURCES_NCMPP = $(wildcard ncmpp/src/*.cpp)
SOURCES_NCMLIB = $(wildcard ncmlib/src/*.cpp)

# Object files
OBJECTS_NCMPP = $(SOURCES_NCMPP:.cpp=.o)
OBJECTS_NCMLIB = $(SOURCES_NCMLIB:.cpp=.o)

# Output binary for ncmpp and static library for ncmlib
OUTPUT_NCMPP = ncmpp/ncmpp
OUTPUT_NCMLIB = ncmlib/libncmlib.a

# Default target
all: $(OUTPUT_NCMLIB) $(OUTPUT_NCMPP)

$(OUTPUT_NCMPP): $(OBJECTS_NCMPP) $(OUTPUT_NCMLIB)
	 $(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJECTS_NCMPP) $(LDFLAGS)

$(OUTPUT_NCMLIB): $(OBJECTS_NCMLIB)
	 ar qc $@ $^
	 ranlib $@

%.o: %.cpp
	 $(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean target
clean:
	 rm -f $(OBJECTS_NCMPP) $(OBJECTS_NCMLIB) $(OUTPUT_NCMPP) $(OUTPUT_NCMLIB)

.PHONY: all clean
