#include <string.h>
#include "Buf.h"

Buf::Buf() {

};
Buf::~Buf() {};
void Buf::appendstr(char *buf_, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (buf_[i] == '\0')
            break;
        buf.push_back(buf_[i]);
    }
}

size_t Buf::buf_size()
{
    return buf.size();
}

void Buf::cleanbuf()
{
    buf.clear();
}

const char *Buf::buf_c_str()
{
    return buf.c_str();
}