
#pragma once


#include "vcl/containers/containers.hpp"


namespace vcl {

/** Special notation vec<N> refers to buffer_stack of N floating values
 * \ingroup math
*/
template <size_t N> using uvec = buffer_stack<unsigned int,N>;

}
