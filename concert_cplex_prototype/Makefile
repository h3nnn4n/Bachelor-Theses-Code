CC=g++
override CFLAGS+=-DIL_STD -c -Wall -g --std=c++11 -I/opt/ibm/ILOG/CPLEX_Studio1263/concert/include -I/opt/ibm/ILOG/CPLEX_Studio1263/cplex/include -Wno-ignored-attributes -Wno-deprecated-declarations
override CFLAGSL+=-lcplex -lilocplex -lconcert -lcplex -lm -lpthread -lgmp -lz
override LDFLAGS+=-DIL_STD -L/opt/ibm/ILOG/CPLEX_Studio1263/cplex/lib/x86-64_linux/static_pic/ -L/opt/ibm/ILOG/CPLEX_Studio1263/concert/lib/x86-64_linux/static_pic/ -lilocplex -lconcert -lcplex -lm -lpthread -lgmp -lz

SOURCES=concert.cpp reader.cpp printer_to_graphviz.cpp backtrack.cpp utils.cpp random.cpp #main.cpp

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
