 This project memory maps a file to memory and accesses it to perform different operations, as needed.
 #mmap_fstream.h : Header file that holds declarations and comments about functionality of each function
 #mmap_fstream.cc : Source file that holds implementation for all functions in the corresponding header file
 
 (a) The file is memory mapped in mmap_fstream.cc on line 85:
 85-    file_contentGlobal = static_cast<char*>(mmap(nullptr, mapped_size, PROT_FLAGS, MAP_SHARED, fd, 0));
 
 (b) The file is ftruncated in mmap_fstream.cc on line 155:
 155-   if(ftruncate(fd, mapped_size) == -1){
 
 (c) The file is read from a memory map in mmap_fstream.cc on line 137:
 137-   *line += file_contentGlobal[cursor];

 (d) The file is written to using a memory map in mmap_fstream.cc on line 160:
 160-   file_contentGlobal[mapped_size-1] = c;

 (e) The file is saved to disk using a memory map in mmap_fstream.cc on line 166:
 166-   if(msync(file_contentGlobal, mapped_size, MS_SYNC) < 0) {
