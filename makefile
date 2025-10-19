EXE ?= c4ke

ifeq ($(OS), Windows_NT)
	SUFFIX := .exe
	STATIC := -static
	OSNAME := win
else
	SUFFIX :=
	STATIC :=
	OSNAME := linux
endif

BIN_X86_64_V1 := c4ke-$(OSNAME)-x86-64$(SUFFIX)
BIN_X86_64_V2 := c4ke-$(OSNAME)-x86-64-v2$(SUFFIX)
BIN_X86_64_V3 := c4ke-$(OSNAME)-x86-64-v3$(SUFFIX)

.PHONY: c4ke mini tcec mini-bench v1 v2 v3 release minifier check tuner clean

c4ke:
	g++ -DNDEBUG -std=c++20 -O3 -march=native -pthread $(STATIC) src/main.cpp -o $(EXE)$(SUFFIX)

mini: minifier
	./mini | ./compress.sh | cat launcher.sh - >c4ke-mini
	chmod +x c4ke-mini
	@echo compressed size: `du -b c4ke-mini`

tcec: minifier
	./mini tcec | ./compress.sh | cat launcher.sh - >c4ke-tcec
	chmod +x c4ke-tcec
	@echo compressed size: `du -b c4ke-tcec`

mini-bench: minifier
	./mini bench
	g++ -DNDEBUG -std=c++20 -O3 -march=native -pthread $(STATIC) mini.cpp -o c4ke-mini-bench$(SUFFIX)

v1:
	g++ -DNDEBUG -std=c++20 -O3 -march=x86-64 -pthread $(STATIC) src/main.cpp -o $(BIN_X86_64_V1)

v2:
	g++ -DNDEBUG -std=c++20 -O3 -march=x86-64-v2 -pthread $(STATIC) src/main.cpp -o $(BIN_X86_64_V2)

v3:
	g++ -DNDEBUG -std=c++20 -O3 -march=x86-64-v3 -pthread $(STATIC) src/main.cpp -o $(BIN_X86_64_V3)

release: clean v1 v2 v3 mini tcec

minifier:
	g++ -O3 -std=c++20 $(STATIC) minifier/*.cpp -o mini$(SUFFIX)

check:
	./check.sh

tuner:
	g++ -DUNICODE -O3 -std=c++20 $(STATIC) tuner/chess/*.cpp tuner/*.cpp -o tune$(SUFFIX)

clean:
	@rm -rf $(EXE)$(SUFFIX)
	@rm -rf c4ke-mini
	@rm -rf c4ke-tcec
	@rm -rf c4ke-mini-bench$(SUFFIX)
	@rm -rf $(BIN_X86_64_V1)
	@rm -rf $(BIN_X86_64_V2)
	@rm -rf $(BIN_X86_64_V3)
	@rm -rf mini$(SUFFIX)
	@rm -rf tune$(SUFFIX)
	@rm -rf mini.cpp
	@rm -rf checkpoint.txt