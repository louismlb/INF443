#include "ivec2.hpp"

#include <iostream>
#include <cassert>

namespace vcl {

ivec2::buffer_stack()
    :x(0),y(0)
{}

ivec2::buffer_stack(int x_arg, int y_arg)
    :x(x_arg),y(y_arg)
{}

const int& ivec2::operator[](std::size_t index) const
{
    switch(index) {
    case 0:
        return x;
    case 1:
        return y;
    default:
        std::cerr<<"Error: Try to access ivec2["<<index<<"]"<<std::endl;
        assert(false);
    }
	abort();
}
int& ivec2::operator[](std::size_t index)
{
    switch(index) {
    case 0:
        return x;
    case 1:
        return y;
    default:
        std::cerr<<"Error: Try to access ivec2["<<index<<"]"<<std::endl;
        assert(false);
    }
	abort();
}

size_t ivec2::size() const
{
    return 2;
}


const int& ivec2::operator()(std::size_t index) const
{
    return (*this)[index];
}
int& ivec2::operator()(std::size_t index)
{
    return (*this)[index];
}

int const& ivec2::at(std::size_t index) const
{
    return (*this)[index];
}
int& ivec2::at(std::size_t index)
{
    return (*this)[index];
}


int* ivec2::begin() { return &x; }
int* ivec2::end() { return &y+1; }
int const* ivec2::begin() const { return &x; }
int const* ivec2::end() const { return &y+1; }
int const* ivec2::cbegin() const { return &x; }
int const* ivec2::cend() const { return &y+1; }



}
