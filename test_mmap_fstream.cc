// Copyright 2024 CSCE 311
//
#include "mmap_fstream.h"
#include "mmap_fstream.h"  // DO NOT DELETE

#include <cstring>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>


// Test functions behave according to their name
bool TestConstructorOpenAndSize(const char[]);
bool TestOpenIsOpenAndGet(const char[]);
bool TestOpenIsOpenPut_newSizeAndGet(const char[]);
bool TestOpenIsOpenSizePutAteAndGet(const char[]);
bool TestOpenIsOpenAndGetline(const char[]);


int main(int argc, char* argv[]) {
  // create an array of function pointers to call above functions
  const std::vector<bool (*)(const char[])> kFstreamTests{
    TestConstructorOpenAndSize,
    TestOpenIsOpenAndGet,
    TestOpenIsOpenAndGetline,
    TestOpenIsOpenPut_newSizeAndGet,
    TestOpenIsOpenSizePutAteAndGet
  };


  // map argv[1] to one of the function pointers
  bool passed = kFstreamTests.at(::atoi(argv[1]))(argc == 3 ? argv[2] : "");
  if (passed) {
    std::cout << "Result: " << "PASSED" << std::endl;

    return 0;
  } else {
    std::cout << "Result: " << "FAILED" << std::endl;

    return 1;
  }
}


bool TestConstructorOpenAndSize(const char kTestValues[]) {
  const std::string kTestFileName = "dat/test_open_file.txt";
  const std::size_t kTestSize = ::strlen(kTestValues);
  bool passed = true;

  std::cout << "TestConstructorOpenAndSize" << std::endl;

  // open file with constructor
  mem_map::fstream fstream(kTestFileName, std::ios_base::in);

  // does file test open
  std::cout << "\tExpected is_open(1): true" << std::endl;
  std::cout << std::boolalpha;  // print bool as text
  std::cout << "\tActual is_open(1): " << fstream.is_open() << std::endl;
  passed = fstream.is_open();

  // is file size correct
  std::cout << "\tExpected file size(1): " << kTestSize << std::endl;
  std::cout << "\tActual file size(1): " << fstream.size() << std::endl;
  passed = fstream.size() == kTestSize && passed;

  // calling open on an open file does nothing
  fstream.open(kTestFileName);

  // does file test open
  std::cout << "\tExpected is_open(2): true" << std::endl;
  std::cout << "\tActual is_open(2): " << fstream.is_open() << std::endl;
  passed = fstream.is_open() && passed;

  // is file size correct
  std::cout << "\tExpected file size(2): " << kTestSize << std::endl;
  std::cout << "\tActual file size(2): " << fstream.size() << std::endl;
  passed = fstream.size() == kTestSize && passed;


  fstream.close();
  // does file test closed
  std::cout << "\tExpected is_open(3): false" << std::endl;
  std::cout << "\tActual is_open(3): " << fstream.is_open() << std::endl;
  passed = !fstream.is_open() && passed;

  return passed;
}


bool TestOpenIsOpenAndGet(const char kTestValues[]) {
  const std::size_t kTestSize = ::strlen(kTestValues);
  const std::string kTestFileName = "dat/test_get_file.txt";
  bool passed = true;

  std::cout << "TestOpenIsOpenAndGet" << std::endl;

  // use default constructor and test open
  mem_map::fstream fstream;
  fstream.open(kTestFileName, std::ios_base::in);
  passed = fstream.is_open();
  std::cout << "\tExpected is_open: true" << std::endl;
  std::cout << std::boolalpha;
  std::cout << "\tActual is_open: " << passed << std::endl;

  // test mmap::fstream::get until end of file (returns \0)
  std::size_t i = 0;
  char c = fstream.get();  // call mmap::fstream::get
  while (c) {  // run while c != \0
    if (i >= kTestSize) {  // check bounds
      passed = false;
      std::cout << "\tExpected get: 0, Actual get: " << static_cast<int>(c)
        << std::endl;
      break;
    }

    passed = passed && kTestValues[i] == c;

    std::cout << "\tExpected get: " << kTestValues[i] << ", Actual get: " << c
      << std::endl;

    c = fstream.get();  // call mmap::fstream::get
    ++i;  // update bounds count
  }

  fstream.close();

  return passed;
}



bool TestOpenIsOpenAndGetline(const char _[]) {
  const std::string kTestFileName = "dat/test_getline_file.txt";
  const char *kTestValues[] = { "CSCE311", "001", "002" };
  const std::size_t kTestSize = 3;
  bool passed = true;

  std::cout << "TestOpenIsOpenAndGetline" << std::endl;

  // use default constructor and test open and eof
  mem_map::fstream fstream;
  // an unopened file is at eof
  std::cout << "\tExpected fstream.eof(1): true" << std::endl;
  std::cout << std::boolalpha;
  std::cout << "\tActual fstream.eof(1): " << fstream.eof() << std::endl;
  passed = passed && fstream.eof();

  fstream.open(kTestFileName, std::ios_base::in);

  std::cout << "\tExpected is_open: true" << std::endl;
  std::cout << std::boolalpha;
  std::cout << "\tActual is_open: " << fstream.is_open() << std::endl;
  passed = passed && fstream.is_open();

  // a newly opened file is not at eof
  std::cout << "\tExpected fstream.eof(2): false" << std::endl;
  std::cout << std::boolalpha;
  std::cout << "\tActual fstream.eof(2): " << fstream.eof() << std::endl;
  passed = passed && !fstream.eof();

  std::string line;
  std::size_t i = 0;
  do {
    fstream.getline(&line);
    
    std::cout << "\tExpected get: \"" << kTestValues[i]
      << "\", Actual get: \"" << line << "\"" << std::endl;
    passed = passed && kTestValues[i] == line;

    ++i;
    if (i < kTestSize && fstream.eof()) {
      // not last line; must not be at eof
      std::cout << "\tExpected fstream.eof(" << (i + 2) << "): false"
        << std::endl;
      std::cout << "\tActual fstream.eof(" << (i + 2) << "): true" << std::endl;
      passed = false;
    } else if (i >= kTestSize && !fstream.eof()) {
      // last line read; must be at eof
      std::cout << "\tExpected fstream.eof(" << (i + 2) << "): true"
        << std::endl;
      std::cout << "\tActual fstream.eof(" << (i + 2) << "): false"
        << std::endl;
      passed = false;
    }
  } while (i < kTestSize);
    
  fstream.close();

  return passed;
}


bool TestOpenIsOpenPut_newSizeAndGet(const char values[]) {
  const std::string kTestFileName = "dat/test_put.txt";
  const std::size_t kTestSize = 4;
  const char kTestValues[] = {'a', 'b', 'c', 'd'};
  bool passed = true;

  std::cout << "TestOpenIsOpenPut_newSizeAndGet" << std::endl;

  // open file
  mem_map::fstream fstream_out(kTestFileName);
  passed = fstream_out.is_open();
  std::cout << "\tExpected is_open: true" << std::endl;
  std::cout << std::boolalpha;  // print bool as text
  std::cout << "\tActual is_open: " << fstream_out.is_open() << std::endl;

  // write to file
  for (std::size_t i = 0; i < kTestSize; ++i)
    fstream_out.put(kTestValues[i]);

  // close to reopen in new instance
  fstream_out.close();

  // open file to read results of write
  mem_map::fstream fstream_in(kTestFileName, std::ios_base::in);
  passed = fstream_in.size() == kTestSize && passed;
  std::cout << "\tExpected size: " << kTestSize << std::endl;
  std::cout << "\tActual size: " << fstream_in.size() << std::endl;

  std::size_t i = 0;
  char c = fstream_in.get();  // call mmap::fstream::get
  while (c) {  // run while c != \0
    if (i >= kTestSize) {  // check bounds
      passed = false;
      std::cout << "\tExpected get: 0, Actual get: " << static_cast<int>(c)
        << std::endl;
      break;
    }

    passed = kTestValues[i] == c && passed;

    std::cout << "\tExpected get: " << kTestValues[i]
      << ", Actual get: " << c << std::endl;

    c = fstream_in.get();  // get next character
    ++i;  // update bounds count
  }

  fstream_in.close();

  return passed;
}


bool TestOpenIsOpenSizePutAteAndGet(const char kTestValues[]) {
  const std::string kTestFileName = "dat/test_ate.txt";
  const std::size_t kTestSize = ::strlen(kTestValues);
  const std::size_t kResultSize = 16;
  const char kResultValues[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
  bool passed = true;

  std::cout << "TestSizeIsOpenPutAteAndGet" << std::endl;

  // open file
  mem_map::fstream fstream_out(
    kTestFileName,
    std::ios_base::in | std::ios_base::out | std::ios_base::ate);
  passed = fstream_out.is_open();
  std::cout << "\tExpected is_open: true" << std::endl;
  std::cout << std::boolalpha;  // print bool as text
  std::cout << "\tActual is_open: " << fstream_out.is_open() << std::endl;

  // write to file
  for (std::size_t i = 0; i < kTestSize; ++i)
    fstream_out.put(kTestValues[i]);

  fstream_out.close();

  // open file to read results of write
  mem_map::fstream fstream_in(kTestFileName, std::ios_base::in);
  passed = fstream_in.size() == kResultSize && passed;
  std::cout << "\tExpected size: " << kResultSize << std::endl;
  std::cout << "\tActual size: " << fstream_in.size() << std::endl;

  for (std::size_t i = 0; i < 10; ++i)
    fstream_in.get();

  std::size_t i = 0;
  char c = fstream_in.get();  // call mmap::fstream::get
  while (c) {  // run while c != \0
    if (i >= kResultSize) {  // check bounds
      passed = false;
      std::cout << "\tExpected get: 0, Actual get: " << static_cast<int>(c)
        << std::endl;
      break;
    }

    passed = passed && kResultValues[i] == c;

    std::cout << "\tExpected get: " << kResultValues[i]
      << ", Actual get: " << c << std::endl;

    c = fstream_in.get();  // get next character
    ++i;  // update bounds count
  }

  return passed;
}

