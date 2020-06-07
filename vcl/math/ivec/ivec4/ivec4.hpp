#pragma once

#include "vcl/containers/buffer_stack/buffer_stack.hpp"

namespace vcl {

/** vec4 is an alias on the generic buffer_stack<float, 4>
 *  \ingroup math
*/
using ivec4 = buffer_stack<int, 4>;

/** vec4 models a lightweight (x,y,z,w) 4D coordinates vector or point.
 * vec4 is a specialized-template class from a generic buffer_stack<type,N>, with type=float and N=4
 * \ingroup math
*/
template <> struct buffer_stack<int, 4> {

    // Public attributes:
    int x; /**< x-coordinate or vec4[0]  */
    int y; /**< y-coordinate or vec4[1]  */
    int z; /**< z-coordinate or vec4[2]  */
    int w; /**< w-coordinate or vec4[3]  */

    /** \name Constructors */
    ///@{
    /** Empty constructor initialize vec4=(0,0,0,0) */
    buffer_stack<int, 4>();
    /** Direct constructor.
     * vec4(x,y,z,w), or vec4{x,y,z,w}, or vec4 p = {x,y,z,w}; */
    buffer_stack<int, 4>(int x,int y,int z,int w);
    ///@}

    /** Return 4 */
    size_t size() const;

    /** \name Element access
     * \brief  Allow vec4[0/1/2/3], or vec4(0/1/2/3), or vec4.at(0/1/2/3) */
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
