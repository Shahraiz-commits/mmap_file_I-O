# Copyright 2024 CSCE 311

cc = g++  # use g++ compiler
path = ..
standard = c++20

flags = -std=$(standard)  # compile with standard
flags += -Wall  # compile with all warnings
flags += -pedantic  # even more warnings
flags += -I $(path)  # add parent to preprocessor include path
flags += -g  # include gdb instrumentation

link = $(cc) $(linker_flags) -o  # final linked build to binary executable

compile = $(cc) $(flags) -c -o  # compilation to intermediary .o files

# tests
#
test-all: test-open test-get test-getline test-put-new test-put-ate \
          find-comments
	@echo 'ALL TESTS RAN'

test-open: test-mmap-fstream
	@echo "\nTEST OPEN"
	@rm -f dat/test_open_file.txt
	@echo -n "0123456789ABCDEF" > dat/test_open_file.txt
	@chmod 664 dat/test_open_file.txt
	./test-mmap-fstream 0 0123456789ABCDEF
	

test-get: test-mmap-fstream
	@echo "\nTEST GET"
	@rm -f dat/test_get_file.txt
	@echo -n "CSCE311" > dat/test_get_file.txt
	@chmod 664 dat/test_get_file.txt
	./test-mmap-fstream 1 CSCE311
	@echo ''

test-getline: test-mmap-fstream
	@echo "\nTEST GETLINE"
	@rm -f dat/test_getline_file.txt
	@echo -n "CSCE311\n001\n002" > dat/test_getline_file.txt
	@chmod 664 dat/test_getline_file.txt
	./test-mmap-fstream 2
	@echo ''

test-put-new: test-mmap-fstream
	@echo "\nTEST PUT (new file)"
	@rm -f dat/test_put.txt
	./test-mmap-fstream 3 01234567890ABCDEF
	@echo ''

test-put-ate: test-mmap-fstream
	@echo "\nTEST PUT (append to file)"
	@echo -n "0123456789" > dat/test_ate.txt
	./test-mmap-fstream 4 "abcdef"
	@echo ''

find-comments:
	@echo "\nTEST FILES FOR COMMENTS"
	@echo MEMORY MAP OPEN FILE:
	@grep -wns "MEMORY MAP OPEN FILE" mmap_fstream.cc -A 2
	@echo "\nFILE MEMORY ALLOCAION:"
	@grep -wns "ALLOCATE FILE MEMORY" mmap_fstream.cc -A 2
	@echo "\nFILE READ:"
	@grep -wns "READ FROM FILE" mmap_fstream.cc -A 2
	@echo "\nFILE WRITE:"
	@grep -wns "SAVE TO DISK" mmap_fstream.cc -A 2
	@echo ''


# executable
#
test-mmap-fstream: test_mmap_fstream.o mmap_fstream.o
	$(link) $@ $^

# objects
#
test_mmap_fstream.o : test_mmap_fstream.cc mmap_fstream.h
	$(compile) $@ $<
#
mmap_fstream.o : mmap_fstream.cc mmap_fstream.h
	$(compile) $@ $<


# utils
#
clean:
	$(RM) -f *.o test-mmap-fstream \
  dat/test_ate.txt dat/test_get.txt dat/test_open.txt dat/test_put.txt
