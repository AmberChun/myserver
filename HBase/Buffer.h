#pragma once

#include "base.h"

class Buffer
{
public:
    Buffer(int buffInitSize = 1024);
    ~Buffer();

    size_t writableSize();
    size_t readableSize();

    size_t readFd(int fd);

    void append(const char * appendbuf,size_t len);

    std::string retrieveAsString();
    void retrieveString(size_t len);

    void checkWasteSpace();

    char * head();

private:
    typedef std::vector<char> BuffVec;
    BuffVec buffvec;
    size_t readIndex;
    size_t writeIndex;
};