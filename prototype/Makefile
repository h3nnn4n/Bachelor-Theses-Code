CC=g++
CFLAGS=-c -Wall -g --std=c++11
CFLAGSL=
LDFLAGS=

SOURCES=reader.cpp printer_to_graphviz.cpp main.cpp backtrack.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=magic

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(CFLAGSL)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ $(CFLAGSL)

.PHONY: clean
clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
