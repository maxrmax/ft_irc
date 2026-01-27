#include "InputBuffer.hpp"

InputBuffer::InputBuffer() {}

void InputBuffer::append(const std::string& data)
{
    _buffer += data;
}

bool InputBuffer::hasLine() const
{
    return _buffer.find("\r\n") != std::string::npos;
}

std::string InputBuffer::popLine()
{
    std::string line;
    size_t pos;

    pos = _buffer.find("\r\n");
    if (pos == std::string::npos)
        return "";

    line = _buffer.substr(0, pos);
    _buffer.erase(0, pos + 2);
    return line;
}
