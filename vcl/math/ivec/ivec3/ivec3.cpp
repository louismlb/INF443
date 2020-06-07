#include "ivec3.hpp"

#include <iostream>
#include "vcl/base/base.hpp"

namespace vcl {

ivec3::buffer_stack()
    :x(0),y(0),z(0)
{}

ivec3::buffer_stack(int x_arg, int y_arg, int z_arg)
    :x(x_arg),y(y_arg),z(z_arg)
{}

const int& ivec3::operator[](std::size_t index) const
{
    switch(index) {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    default:
        error_vcl("Try to access ivec3["+std::to_string(index)+"]");
    }

}
int& ivec3::operator[](std::size_t index)
{
    switch(index) {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    default:
        error_vcl("Try to access ivec3["+std::to_string(index)+"]");
    }

}

size_t ivec3::size() const
{
    return 3;
}


const int& ivec3::operator()(std::size_t index) const
{
    return (*this)[index];
}
int& ivec3::operator()(std::size_t index)
{
    return (*this)[index];
}
int const& ivec3::at(std::size_t index) const
{
    return (*this)[index];
}
int& ivec3::at(std::size_t index)
{
    return (*this)[index];
}


int* ivec3::begin()
{
    return &x;
}
int* ivec3::end()
{
    return &z+1;
}
int const* ivec3::begin() const
{
    return &x;
}
int const* ivec3::end() const
{
    return &z+1;
}
int const* ivec3::cbegin() const
{
    return &x;
}
int const* ivec3::cend() const
{
    return &z+1;
}

ivec3 cross(const ivec3& a,const ivec3& b)
{
    return    { a.y*b.z-a.z*b.y,
                a.z*b.x-a.x*b.z,
                a.x*b.y-a.y*b.x };
}

}
