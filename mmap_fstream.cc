// Shahraiz Aziz
#include "mmap_fstream.h"
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

namespace mem_map {

fstream::fstream(){
    open_state = false;
    emptyFile = false;
    fd = -1;
    mapped_size = 0;
    file_contentGlobal = nullptr;
    cursor = 0;
    flags = 0;
    PROT_FLAGS = 0;
}

fstream::fstream(const std::string& fname) :fstream() {
    open(fname, (std::ios_base::in | std::ios_base::out));
}
fstream::fstream(const std::string& fname, std::ios_base::openmode mode ) :fstream() {
     open(fname, mode);
}

void fstream::open(const std::string& fname, std::ios_base::openmode mode) { 
    if(is_open()) return;
    if(mode == std::ios_base::ate){
         flags |= O_APPEND | O_CREAT;
         PROT_FLAGS |= PROT_WRITE;
    }
    if(mode == std::ios_base::in) {
        flags |= O_RDONLY;
        PROT_FLAGS |= PROT_READ;
    }
    if(mode == std::ios_base::out) {
        flags |= O_WRONLY;
        PROT_FLAGS |= PROT_WRITE;
    }
    if(mode == (std::ios_base::in | std::ios_base::out)){
        flags |= O_RDWR | O_CREAT;
        PROT_FLAGS |= PROT_READ | PROT_WRITE;
    } 
    if(mode == (std::ios_base::in | std::ios_base::ate)){
        flags |= O_RDWR | O_APPEND | O_CREAT;
        PROT_FLAGS |= PROT_READ | PROT_WRITE;
    }
    if(mode == (std::ios_base::out | std::ios_base::ate)){
        flags |= O_WRONLY | O_APPEND | O_CREAT;
        PROT_FLAGS |= PROT_WRITE;
    }
    if(mode == (std::ios_base::in | std::ios_base::out | std::ios_base::ate)){
        flags |= O_RDWR | O_CREAT | O_APPEND;
        PROT_FLAGS |= PROT_READ | PROT_WRITE;
    }

    bool cursorAtEnd = false;
    if (mode & std::ios_base::ate) cursorAtEnd = true;

    fd = ::open(fname.c_str(), flags, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        std::cerr << "Failed to open file\n";
        return;
    }
    open_state = true;

    // Get file info
    struct stat file_info;
    if (fstat(fd, &file_info) == -1) {
        std::cerr << "Failed to get file information: " << std::endl;
        close();
        return;
    }
    // Get the file size from the file info
    mapped_size = file_info.st_size;
    
    if(mapped_size==0) {
        mapped_size = 1; // For empty files to be mapped (cant be mapped with size=0)
        emptyFile = true;
    }
    
    // MEMORY MAP OPEN FILE
    file_contentGlobal = static_cast<char*>(mmap(nullptr, mapped_size, PROT_FLAGS, MAP_SHARED, fd, 0));
    if (file_contentGlobal == MAP_FAILED) {
        std::cerr << "Failed to map file into memory\n";
        close();
        return;
    }
//std::cout << "filecontentNEW: " << file_contentGlobal[0] << std::endl;
    if(cursorAtEnd) cursor = mapped_size;
    else cursor = 0;
    
}

void fstream::close() {
    if (file_contentGlobal != nullptr) {
        munmap(file_contentGlobal, mapped_size);
        file_contentGlobal = nullptr;
    }

    if (fd != -1) {
        ::close(fd);
        fd = -1;
    }
    open_state = false;
}

bool fstream::is_open() const {
    return open_state;
}

bool fstream::eof() const {
    if(cursor == mapped_size || !is_open()) return true;
    return false;

}

std::size_t fstream::size() const {
    return mapped_size;
}

char fstream::get() {
    if(!eof()){
        cursor++;
        return file_contentGlobal[cursor-1];
    }
    else return '\0';
    
}

fstream& fstream::getline(std::string* line) {
    line -> clear();
    while (cursor < mapped_size && file_contentGlobal[cursor] != '\n') {
         // READ FROM FILE
        *line += file_contentGlobal[cursor];
        cursor++;
    }

    // Move cursor to the next line
    if (cursor < mapped_size && file_contentGlobal[cursor] == '\n') {
        cursor++;
    }
    return *this;
}

fstream& fstream::put(char c) {
    if(emptyFile){
        mapped_size = 0;
        emptyFile = false;
    }
    mapped_size++;
    // ALLOCATE FILE MEMORY
    if(ftruncate(fd, mapped_size) == -1){
        std::cerr << "Could not truncate file";
    }

    // WRITE TO FILE
    file_contentGlobal[mapped_size-1] = c;
    
    // update cursor
    cursor = mapped_size;

    // SAVE TO DISK
    if(msync(file_contentGlobal, mapped_size, MS_SYNC) < 0) {
        printf("msync err: %s", strerror(errno));
    }
    
    return *this;
}

} // namespace mem_map
