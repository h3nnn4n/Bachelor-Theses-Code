CC=g++
override CFLAGS+=-c -Wall -g --std=c++11 -I/opt/ibm/ILOG/CPLEX_Studio1263/cplex/include
override CFLAGSL+=-lcplex
override LDFLAGS+=-lgmp -lcplex -lpthread -lz -L/opt/ibm/ILOG/CPLEX_Studio1263/cplex/lib/x86-64_linux/static_pic/

SOURCES=reader.cpp printer_to_graphviz.cpp main.cpp backtrack.cpp utils.cpp random.cpp

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
