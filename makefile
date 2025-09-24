EXE ?= c4ke

ifeq ($(OS), Windows_NT)
	SUFFIX := .exe
	STATIC := -static
else
	SUFFIX :=
	STATIC :=
endif

EXE := $(EXE)$(SUFFIX)

.PHONY: c4ke c4ke-mini c4ke-mini-bench minifier check tuner clean

c4ke:
	g++ -DNDEBUG -std=c++20 -O3 -march=native -pthread $(STATIC) src/main.cpp -o $(EXE)

c4ke-mini: minifier
	./mini | ./compress.sh | cat launcher.sh - >c4ke-mini
	chmod +x c4ke-mini
	@echo compressed size: `du -b c4ke-mini`

c4ke-mini-bench: minifier
	./mini bench
	g++ -DNDEBUG -std=c++20 -O3 -march=native -pthread $(STATIC) mini.cpp -o c4ke-mini-bench$(SUFFIX)

minifier:
	g++ -O3 -std=c++20 $(STATIC) minifier/*.cpp -o mini$(SUFFIX)

check:
	./check.sh

tuner:
	g++ -DUNICODE -O3 -std=c++20 $(STATIC) tuner/chess/*.cpp tuner/*.cpp -o tune$(SUFFIX)

clean:
	@rm -rf $(EXE)
	@rm -rf c4ke-mini
	@rm -rf c4ke-mini-bench$(SUFFIX)
	@rm -rf mini$(SUFFIX)
	@rm -rf tune$(SUFFIX)
	@rm -rf mini.cpp
	@rm -rf checkpoint.txt