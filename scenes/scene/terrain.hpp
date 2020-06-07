#pragma once

#include "main/scene_base/base.hpp"
#include "terrain_creation.hpp"
#include "marching_cubes_cpu.hpp"


// Stores some parameters that can be set from the GUI
struct gui_scene_structure
{
    bool wireframe   = false;
    bool surface     = true;
};

// Store a vec3 (p) + time (t)
struct vec3t{
    vcl::vec3 p; // position
    float t;     // time
};

struct scene_model : scene_base
{

    /** A part must define two functions that are called from the main function:
     * setup_data: called once to setup data before starting the animation loop
     * frame_draw: called at every displayed frame within the animation loop
     *
     * These two functions receive the following parameters
     * - shaders: A set of shaders.
     * - scene: Contains general common object to define the 3D scene. Contains in particular the camera.
     * - data: The part-specific data structure defined previously
     * - gui: The GUI structure allowing to create/display buttons to interact with the scene.
    */

    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);

    void set_gui();

    // visual representation of a surface
    vcl::mesh_drawable terrain;
    GLuint terrain_texture;

    MeshGeneratorCPU terraingen;

    //mesh_generator terraingen_2;

    int counter;

    gui_scene_structure gui_scene;

};



