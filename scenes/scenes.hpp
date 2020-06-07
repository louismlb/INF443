
#pragma once

// All scenes must be included in this file
//  If you add a new scene: its corresponding header file must be included
//  This can be done manually or using the automatic script

#include "scenes/scene/terrain.hpp" // scene file
//#include "scenes/scene/terrain_creation.hpp" // Heightmap erosion, only CPU
//#include "scenes/scene/marching_cubes.hpp" // Marching cubes GPU not working
#include "scenes/scene/marching_cubes_cpu.hpp" // Marching cubes CPU
