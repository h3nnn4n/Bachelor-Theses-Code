CC=g++
override CFLAGS+=-DIL_STD -c -Wall -Og -g --std=c++11 -Wno-ignored-attributes -Wno-deprecated-declarations
override CFLAGSL+=-lm
override LDFLAGS+=-lm

SOURCES=                          \
    main.cpp                      \
    reader.cpp                    \
    utils.cpp    	          \
    random.cpp

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
