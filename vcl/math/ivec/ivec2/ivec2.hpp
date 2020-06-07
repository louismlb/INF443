#pragma once

#include "vcl/containers/buffer_stack/buffer_stack.hpp"


namespace vcl {

/** vec2 is an alias on a generic buffer_stack<float, 2>
 *  \ingroup math */
using ivec2 = buffer_stack<int, 2>;

/** vec2 models a lightweight (x,y) 2D coordinates vector or point.
 * vec2 is a specialized-template class from a generic buffer_stack<type,N>, with type=float and N=2
 * \ingroup math
 */
template <> struct buffer_stack<int, 2> {

    int x; /**< x-coordinate or vec2[0]  */
    int y; /**< y-coordinate or vec2[1] */

    /** \name Constructors */
    ///@{
    /** Empty constructor initialize vec2=(0,0) */
    buffer_stack<int, 2>();
    /** Direct constructor.
     * vec2(x,y), or vec2{x,y}, or vec2 p = {x,y}; */
    buffer_stack<int, 2>(int x,int y);
    ///@}

    /** Return 2 */
    size_t size() const;

    /** \name Element access
     * \brief  Allow vec2[0/1], or vec2(0/1), or vec2.at(0/1) */
    ///@{
    const int& operator[](std::size_t index) const;
    int& operator[](std::size_t index);

    const int& operator()(std::size_t index) const;
    int& operator()(std::size_t index);

    int const& at(std::size_t index) const;
    int& at(std::size_t index);
    ///@}

    /** \name Iterators
     * Iterators are direct pointers on data */
    ///@{
    int* begin();
    int* end();
    int const* begin() const;
    int const* end() const;
    int const* cbegin() const;
    int const* cend() const;
    ///@}
};



}
