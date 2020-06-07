#include "uvec3.hpp"

#include <iostream>
#include "vcl/base/base.hpp"

namespace vcl {

uvec3::buffer_stack()
    :x(0),y(0),z(0)
{}

uvec3::buffer_stack(unsigned int x_arg, unsigned int y_arg, unsigned int z_arg)
    :x(x_arg),y(y_arg),z(z_arg)
{}

const unsigned int& uvec3::operator[](std::size_t index) const
{
    switch(index) {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    default:
        error_vcl("Try to access uvec3["+std::to_string(index)+"]");
    }

}
unsigned int& uvec3::operator[](std::size_t index)
{
    switch(index) {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    default:
        error_vcl("Try to access uvec3["+std::to_string(index)+"]");
    }

}

size_t uvec3::size() const
{
    return 3;
}


const unsigned int& uvec3::operator()(std::size_t index) const
{
    return (*this)[index];
}
unsigned int& uvec3::operator()(std::size_t index)
{
    return (*this)[index];
}
unsigned int const& uvec3::at(std::size_t index) const
{
    return (*this)[index];
}
unsigned int& uvec3::at(std::size_t index)
{
    return (*this)[index];
}


unsigned int* uvec3::begin()
{
    return &x;
}
unsigned int* uvec3::end()
{
    return &z+1;
}
unsigned int const* uvec3::begin() const
{
    return &x;
}
unsigned int const* uvec3::end() const
{
    return &z+1;
}
unsigned int const* uvec3::cbegin() const
{
    return &x;
}
unsigned int const* uvec3::cend() const
{
    return &z+1;
}

uvec3 cross(const uvec3& a,const uvec3& b)
{
    return    { a.y*b.z-a.z*b.y,
                a.z*b.x-a.x*b.z,
                a.x*b.y-a.y*b.x };
}

}
