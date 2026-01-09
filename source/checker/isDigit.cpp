#include "../../includes/ircserv.hpp"

//is a string all digits
bool is_digits_only(const std::string &string)
{
    //std::all_of checks that all characters in the string satisfy a condition.
    //::isdigit returns true if a character is a digit.
    // The !s.empty() prevents empty strings from returning true.
    return !string.empty() && std::all_of(string.begin(), string.end(), ::isdigit);
}