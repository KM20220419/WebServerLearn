#pragma once

#include <string>
class Buf
{
public:
    Buf();
    ~Buf();
    void appendstr(char *buf, int size);
    size_t buf_size();
    void cleanbuf();
    const char *buf_c_str();

private:
    std::string buf;
};
