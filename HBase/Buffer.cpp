#include "Buffer.h"

Buffer::Buffer(int buffInitSize)
:readIndex(0)
,writeIndex(0)
{
    buffvec.resize(buffInitSize);
}

Buffer::~Buffer()
{

}

size_t Buffer::writableSize()
{
    assert(writeIndex <= buffvec.size());
    return buffvec.size() - writeIndex;
}

size_t Buffer::readableSize()
{
    assert(writeIndex <= buffvec.size());
    return writeIndex - readIndex;
}

size_t Buffer::readFd(int fd)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableSize();
    vec[0].iov_base = &*(buffvec.begin() + writeIndex);
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);
    if(size_t(n) <= writable)
    {
        writeIndex += n;
    }
    else if(size_t(n) > writable) 
    {
        append(extrabuf,n - writable);
    }

    return n;
}

void Buffer::append(const char * appendbuf,size_t len)
{
    std::copy(appendbuf,appendbuf + len,&*(buffvec.begin() + writeIndex));
}

std::string Buffer::retrieveAsString()
{
    size_t index = buffvec.size();
    for(size_t i = readIndex; i < writeIndex && i < buffvec.size() ;++i)
    {
        if(buffvec[i] == ';')
        {
            index = i;
            break;
        }
    }

    if(index < buffvec.size())
    {
        std::string result(buffvec.begin() + readIndex,buffvec.begin() + index);
        readIndex = index + 1;
        checkWasteSpace();
        return result;
    }

    return "";
}

void Buffer::retrieveString(size_t len)
{
    readIndex += len;
    if(readIndex > writeIndex)
    {
        readIndex = 0;
        writeIndex = 0;
    }
}

void Buffer::checkWasteSpace()
{
    if(readIndex == writeIndex)
    {
        readIndex = 0;
        writeIndex = 0;
    }

    if(readIndex > 1024)
    {
        BuffVec tempVec(buffvec.begin() + readIndex,buffvec.begin() + writeIndex);
        buffvec.swap(tempVec);
        readIndex = 0;
        writeIndex = buffvec.size();
    }
}

char * Buffer::head()
{
    return &*(buffvec.begin() + readIndex);
}
