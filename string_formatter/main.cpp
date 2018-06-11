#include <iostream>
#include <string>

#include "string_format.h"

using std::cout;
using std::endl;
using std::cin;

using string_format::format;

class hello_world
{
    friend std::ostream& operator << (std::ostream& os, const hello_world& rhs)
    {
        os << "Hello world";
        return os;
    }
};

int main(int argc, char** argv)
{
    cout << format("{3:F1} + {1}.{1} = {2}\n{4}{0}", '!', 2, "5", 2.8, hello_world{}) << endl;

    char _1;
    cin >> _1;

    return 0;
}
