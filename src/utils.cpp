#include <ios> 
#include <limits>
#include <cctype>
#include <cpptoml.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ios>
#include <iostream>
#include <limits>

// clearBuffer
void clearBuffer() {
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void checkIfExtractionFailed() {
  if (!std::cin) {          // if previous extraction failed
    if (std::cin.eof()) {   // check if eof and if yes aborts the program
      std::abort();
    }

  std::cin.clear(); // put std::cin back into normal mode
  }
  clearBuffer();    // remove bad input
}
