CC = g++
EXE = build/run.exe
CFLAGS = -o $(EXE)
GTKMM_FLAGS = `pkg-config gtkmm-3.0 --cflags --libs`
DEBUG_FLAG = -D DEBUG -g

.PHONY: compile_debug compile run run_debug clean

compile_debug:
	$(CC) src/main.cpp $(DEBUG_FLAG) $(CFLAGS) $(GTKMM_FLAGS)

compile:
	$(CC) src/main.cpp $(CFLAGS) $(GTKMM_FLAGS)

run:
	make compile
	./$(EXE)

run_debug:
	make compile_debug
	./$(EXE)

clean:
	rm build/*
