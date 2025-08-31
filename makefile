EXE ?= c4ke

ifeq ($(OS), Windows_NT)
	SUFFIX := .exe
	STATIC := -static
else
	SUFFIX :=
	STATIC :=
endif

EXE := $(EXE)$(SUFFIX)

.PHONY: c4ke c4ke-mini minifier clean

c4ke:
	g++ -DNDEBUG -std=c++20 -O3 -march=native -pthread $(STATIC) src/main.cpp -o $(EXE)

c4ke-mini: minifier
	./mini | ./compress.sh | cat launcher.sh - >c4ke-mini
	chmod +x c4ke-mini
	@echo compressed size: `du -b c4ke-mini`

minifier:
	g++ -O3 -std=c++20 $(STATIC) minifier/*.cpp -o mini$(SUFFIX)

clean:
	@rm -rf $(EXE)
	@rm -rf c4ke-mini
	@rm -rf mini$(SUFFIX)
	@rm -rf mini.cpp