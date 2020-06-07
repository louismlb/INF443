#include "uvec4.hpp"

#include <iostream>
#include <cassert>

namespace vcl {

uvec4::buffer_stack()
    :x(0),y(0),z(0),w(0)
{}

uvec4::buffer_stack(unsigned int x_arg, unsigned int y_arg, unsigned int z_arg, unsigned int w_arg)
    :x(x_arg),y(y_arg),z(z_arg),w(w_arg)
{}

const unsigned int& uvec4::operator[](std::size_t index) const
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
        std::cerr<<"Error: Try to access uvec4["<<index<<"]"<<std::endl;
        assert(false);
    }
	abort();
}
unsigned int& uvec4::operator[](std::size_t index)
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
        std::cerr<<"Error: Try to access uvec4["<<index<<"]"<<std::endl;
        assert(false);
    }
	abort();
}
size_t uvec4::size() const
{
    return 4;
}


const unsigned int& uvec4::operator()(std::size_t index) const {return (*this)[index];}
unsigned int& uvec4::operator()(std::size_t index) {return (*this)[index];}

unsigned int const& uvec4::at(std::size_t index) const {return (*this)[index];}
unsigned int& uvec4::at(std::size_t index) {return (*this)[index];}


unsigned int* uvec4::begin() {return &x;}
unsigned int* uvec4::end() {return &w+1;}
unsigned int const* uvec4::begin() const {return &x;}
unsigned int const* uvec4::end() const {return &w+1;}
unsigned int const* uvec4::cbegin() const {return &x;}
unsigned int const* uvec4::cend() const {return &w+1;}


}
