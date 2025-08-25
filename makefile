EXE ?= c4ke

ifeq ($(OS), Windows_NT)
	SUFFIX := .exe
	STATIC := -static
else
	SUFFIX :=
	STATIC :=
endif

EXE := $(EXE)$(SUFFIX)

c4ke:
	g++ -DNDEBUG -std=c++20 -O3 -march=native -pthread $(STATIC) src/*.cpp -o $(EXE)

clean:
	@rm -rf $(EXE)