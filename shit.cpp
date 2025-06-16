#include <iostream>
#include <sstream>


int main()
{
    double num = 3.14159;
std::stringstream ss;
ss << num;
std::string str = ss.str();
std::cout << "String representation of the number: " << str << std::endl;
}