// Transpiled from CStar
#include "ext/stdcstar.h"

#include <posix_util.h>
#include <iostream>
#include <fstream>
#include <cstdlib>


int main() {
    
    using namespace std;
    
        cout << "DEBUG A" << endl;
        fstream* file = new fstream("test.txt", ios::in | ios::out | ios::binary | ios::trunc);
        cout << "DEBUG B" << endl;
    
        if (!file->is_open()) {
            cerr << "Failed to open file." << endl;
            return ENOENT;
        }
    
        fd_table.push_back(file);
        int fd = fd_table.size() - 1;
    
        cout << "Write stream pointer: " << file << endl;
        cout << "fd_table[fd] (write): " << fd_table[fd] << endl;
        cout << "fd_table[fd] (read): " << fd_table[fd] << endl;
    
        POSIXSTR content = "Hello, POSIX file I/O!\n";
        filewrite(fd, content, strlen(content));
    
        fd_table[fd]->seekg(0);
    
        char buffer[128] = {0};
        ssize_t bytesRead = fileread("test.txt", fd, buffer, sizeof(buffer));
        if (bytesRead < 0) {
            cerr << "fileread failed with error: " << posix_errno << endl;
        } else {
            cout << "Read " << bytesRead << " bytes\n";
            cout << "Content:\n" << buffer << endl;
        }
    
        file->close();
        delete file;
        fd_table[fd] = nullptr;
        return EXIT_S;
}

