# c4ke
chess 4k engine

## Minifier
The minifier isn't general purpose and won't work on all c++ code
- Remove whitespaces
- Remove single-line comments
- Remove `#ifdef` blocks (usually for OpenBench compliance code)
- Find and replace `#define` constants
- Rename variables, functions and structs

## Thanks
- Members of the MinusKelvin discord server
- The following engines that I use as references:
  - [ice4](https://github.com/MinusKelvin/ice4)
  - [4ku](https://github.com/kz04px/4ku)
