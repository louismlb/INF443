#include "uvec2.hpp"

#include <iostream>
#include <cassert>

namespace vcl {

uvec2::buffer_stack()
    :x(0),y(0)
{}

uvec2::buffer_stack(unsigned int x_arg, unsigned int y_arg)
    :x(x_arg),y(y_arg)
{}

const unsigned int& uvec2::operator[](std::size_t index) const
{
    switch(index) {
    case 0:
        return x;
    case 1:
        return y;
    default:
        std::cerr<<"Error: Try to access uvec2["<<index<<"]"<<std::endl;
        assert(false);
    }
	abort();
}
unsigned int& uvec2::operator[](std::size_t index)
{
    switch(index) {
    case 0:
        return x;
    case 1:
        return y;
    default:
        std::cerr<<"Error: Try to access uvec2["<<index<<"]"<<std::endl;
        assert(false);
    }
	abort();
}

size_t uvec2::size() const
{
    return 2;
}


const unsigned int& uvec2::operator()(std::size_t index) const
{
    return (*this)[index];
}
unsigned int& uvec2::operator()(std::size_t index)
{
    return (*this)[index];
}

unsigned int const& uvec2::at(std::size_t index) const
{
    return (*this)[index];
}
unsigned int& uvec2::at(std::size_t index)
{
    return (*this)[index];
}


unsigned int* uvec2::begin() { return &x; }
unsigned int* uvec2::end() { return &y+1; }
unsigned int const* uvec2::begin() const { return &x; }
unsigned int const* uvec2::end() const { return &y+1; }
unsigned int const* uvec2::cbegin() const { return &x; }
unsigned int const* uvec2::cend() const { return &y+1; }



}
