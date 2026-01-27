#ifndef INPUTBUFFER_HPP
#define INPUTBUFFER_HPP
#include <string>
#include <vector>

class InputBuffer {
public:
    InputBuffer();

    void append(const std::string& data);
    bool hasLine() const;
    std::string popLine();

private:
    std::string _buffer;
};
#endif // INPUTBUFFER_HPP