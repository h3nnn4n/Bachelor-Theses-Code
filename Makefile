CC=g++
override CFLAGS+=-DIL_STD -c -Wall -Og -g --std=c++11
override CFLAGSL+=-lm -I/home/h3nnn4n/scipoptsuite-3.2.1/scip-3.2.1/src
override LDFLAGS+=-L/home/h3nnn4n/scipoptsuite-3.2.1/scip-3.2.1/lib -lscip.linux.x86_64.gnu.opt \
                  -lobjscip.linux.x86_64.gnu.opt -llpispx.linux.x86_64.gnu.opt -lnlpi.cppad.linux.x86_64.gnu.opt \
                  -O3 -fomit-frame-pointer -mtune=native    -lsoplex.linux.x86_64.gnu.opt \
                  -lm -m64  -lz -lzimpl.linux.x86_64.gnu.opt  -lgmp -lreadline -lncurses -Wl,-rpath,/home/h3nnn4n/scipoptsuite-3.2.1/scip-3.2.1/lib


SOURCES=           \
    main.cpp       \
    reader.cpp     \
    utils.cpp      \
    model.cpp      \
    pricer_vrp.cpp \
    greedy_heur.cpp \
    tabu_search.cpp \
    simmulated_annealing.cpp \
    ant_colony_optimization.cpp \
    meta_heuristics_utils.cpp \
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
