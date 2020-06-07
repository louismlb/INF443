#pragma once

#include "vcl/containers/buffer_stack/buffer_stack.hpp"



namespace vcl {

/** vec3 is an alias on a generic buffer_stack<float, 3>
 *  \ingroup math
*/
using ivec3 = buffer_stack<int, 3>;

/** vec3 models a lightweight (x,y,z) 3D coordinates vector or point.
 * vec3 is a specialized-template class from a generic buffer_stack<type,N>, with type=float and N=3
 * \ingroup math
 * @{
*/
template <> struct buffer_stack<int, 3> {

    // Public attributes:
    int x; /**< x-coordinate or vec3[0]  */
    int y; /**< y-coordinate or vec3[1] */
    int z; /**< z-coordinate or vec3[2] */

    /** \name Constructors */
    ///@{
    /** Empty constructor initialize vec3=(0,0,0) */
    buffer_stack<int, 3>();
    /** Direct constructor.
     * vec3(x,y,z), or vec3{x,y,z}, or vec3 p = {x,y,z}; */
	buffer_stack<int, 3>(int x,int y,int z);
    ///@}

    /** Return 3 */
    size_t size() const;

    /** \name Element access
     * \brief  Allow vec3[0/1/2], or vec3(0/1/2), or vec3.at(0/1/2) */
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

/** \ingroup math
 * @{
 */

/** Cross product between two vec3.
 * \relates buffer_stack<float,3>
*/
ivec3 cross(const ivec3& a,const ivec3& b);

/** @} */

}
