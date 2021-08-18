CC = g++
EXE = build/run.exe
CFLAGS = -o $(EXE)
GTKMM_FLAGS = `pkg-config gtkmm-3.0 --cflags --libs`
DEBUG_FLAG = -D DEBUG -g

build_debug: src/*
	$(CC) src/main.cpp $(DEBUG_FLAG) $(CFLAGS) $(GTKMM_FLAGS)

build: src/*
	$(CC) src/main.cpp $(CFLAGS) $(GTKMM_FLAGS)

run: src/*
	make build
	./$(EXE)

run_debug: src/*
	make build_debug
	./$(EXE)