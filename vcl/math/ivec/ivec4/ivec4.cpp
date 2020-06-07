#include "ivec4.hpp"

#include <iostream>
#include <cassert>

namespace vcl {

ivec4::buffer_stack()
    :x(0),y(0),z(0),w(0)
{}

ivec4::buffer_stack(int x_arg, int y_arg, int z_arg, int w_arg)
    :x(x_arg),y(y_arg),z(z_arg),w(w_arg)
{}

const int& ivec4::operator[](std::size_t index) const
{
    switch(index) {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        std::cerr<<"Error: Try to access ivec4["<<index<<"]"<<std::endl;
        assert(false);
    }
	abort();
}
int& ivec4::operator[](std::size_t index)
{
    switch(index) {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        std::cerr<<"Error: Try to access ivec4["<<index<<"]"<<std::endl;
        assert(false);
    }
	abort();
}
size_t ivec4::size() const
{
    return 4;
}


const int& ivec4::operator()(std::size_t index) const {return (*this)[index];}
int& ivec4::operator()(std::size_t index) {return (*this)[index];}

int const& ivec4::at(std::size_t index) const {return (*this)[index];}
int& ivec4::at(std::size_t index) {return (*this)[index];}


int* ivec4::begin() {return &x;}
int* ivec4::end() {return &w+1;}
int const* ivec4::begin() const {return &x;}
int const* ivec4::end() const {return &w+1;}
int const* ivec4::cbegin() const {return &x;}
int const* ivec4::cend() const {return &w+1;}


}
