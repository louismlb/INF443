#pragma once

#include "vcl/containers/buffer_stack/buffer_stack.hpp"

namespace vcl {

/** vec4 is an alias on the generic buffer_stack<float, 4>
 *  \ingroup math
*/
using uvec4 = buffer_stack<unsigned int, 4>;

/** vec4 models a lightweight (x,y,z,w) 4D coordinates vector or pounsigned int.
 * vec4 is a specialized-template class from a generic buffer_stack<type,N>, with type=float and N=4
 * \ingroup math
*/
template <> struct buffer_stack<unsigned int, 4> {

    // Public attributes:
    unsigned int x; /**< x-coordinate or vec4[0]  */
    unsigned int y; /**< y-coordinate or vec4[1]  */
    unsigned int z; /**< z-coordinate or vec4[2]  */
    unsigned int w; /**< w-coordinate or vec4[3]  */

    /** \name Constructors */
    ///@{
    /** Empty constructor initialize vec4=(0,0,0,0) */
    buffer_stack<unsigned int, 4>();
    /** Direct constructor.
     * vec4(x,y,z,w), or vec4{x,y,z,w}, or vec4 p = {x,y,z,w}; */
    buffer_stack<unsigned int, 4>(unsigned int x,unsigned int y,unsigned int z,unsigned int w);
    ///@}

    /** Return 4 */
    size_t size() const;

    /** \name Element access
     * \brief  Allow vec4[0/1/2/3], or vec4(0/1/2/3), or vec4.at(0/1/2/3) */
    ///@{
    const unsigned int& operator[](std::size_t index) const;
    unsigned int& operator[](std::size_t index);

    const unsigned int& operator()(std::size_t index) const;
    unsigned int& operator()(std::size_t index);

    unsigned int const& at(std::size_t index) const;
    unsigned int& at(std::size_t index);
    ///@}

    /** \name Iterators
     * Iterators are direct pounsigned inters on data */
    ///@{
    unsigned int* begin();
    unsigned int* end();
    unsigned int const* begin() const;
    unsigned int const* end() const;
    unsigned int const* cbegin() const;
    unsigned int const* cend() const;
    ///@}
};



}
