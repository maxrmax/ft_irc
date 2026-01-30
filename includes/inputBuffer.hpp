#ifndef INPUTBUFFER_HPP
#define INPUTBUFFER_HPP

#include <string>

class InputBuffer
{
public:
    InputBuffer();

    void        append(const std::string& data);
    bool        hasLine() const;
    std::string popLine();

private:
    std::string _buffer;

    static const size_t IRC_MAX_LINE = 510; // excluding \r\n
};

#endif // INPUTBUFFER_HPP
