#pragma once

#include "main/scene_base/base.hpp"
#include "terrain.hpp"
#include "bonhomme.hpp"
#include "beeswarm.hpp"

#ifdef SCENE_3D_GRAPHICS



struct gui_scene_structure
{
    bool wireframe = false;
    bool surface = true;
    bool sbdy = true;
    bool bees = true;
    int swarw_behavior = 0;

};

struct scene_model : scene_base
{

    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);

    void set_gui();


    terrain landscape;

    //Bonhomme
    bonhomme Jack;

    //Swarm
    beeswarm miel_pops;

    //Textures
    GLuint texture_id_terrain;
    GLuint texture_id_sky;
    GLuint texture_id_sea;


    gui_scene_structure gui_scene;

    vcl::timer_interval timer;
    //vcl::timer_event timer2;    // Timer allowing to indicate periodic events

    void keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouse_click(scene_structure& scene, GLFWwindow* window, int , int , int );
};



#endif


