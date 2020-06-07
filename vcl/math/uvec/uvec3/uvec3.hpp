#pragma once

#include "vcl/containers/buffer_stack/buffer_stack.hpp"



namespace vcl {

/** vec3 is an alias on a generic buffer_stack<float, 3>
 *  \ingroup math
*/
using uvec3 = buffer_stack<unsigned int, 3>;

/** vec3 models a lightweight (x,y,z) 3D coordinates vector or pounsigned int.
 * vec3 is a specialized-template class from a generic buffer_stack<type,N>, with type=float and N=3
 * \ingroup math
 * @{
*/
template <> struct buffer_stack<unsigned int, 3> {

    // Public attributes:
    unsigned int x; /**< x-coordinate or vec3[0]  */
    unsigned int y; /**< y-coordinate or vec3[1] */
    unsigned int z; /**< z-coordinate or vec3[2] */

    /** \name Constructors */
    ///@{
    /** Empty constructor initialize vec3=(0,0,0) */
    buffer_stack<unsigned int, 3>();
    /** Direct constructor.
     * vec3(x,y,z), or vec3{x,y,z}, or vec3 p = {x,y,z}; */
    buffer_stack<unsigned int, 3>(unsigned int x,unsigned int y,unsigned int z);
    ///@}

    /** Return 3 */
    size_t size() const;

    /** \name Element access
     * \brief  Allow vec3[0/1/2], or vec3(0/1/2), or vec3.at(0/1/2) */
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

/** \ingroup math
 * @{
 */

/** Cross product between two vec3.
 * \relates buffer_stack<float,3>
*/
uvec3 cross(const uvec3& a,const uvec3& b);

/** @} */

}
