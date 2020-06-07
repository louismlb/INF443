#pragma once

#include "main/scene_base/base.hpp"

struct gui_bonhomme_structure
{
    bool wireframe   = false;
    bool surface     = true;
    bool skeleton    = false;
    bool third_person = false;

    bool running = false;
    int turn = 0;

    void set_gui_bonhomme(bool surf, bool wire);
};


struct bonhomme
{
    void setup_dude(std::map<std::string,GLuint>& shaders);
    void draw_dude(std::map<std::string,GLuint>& shaders, scene_structure& scene, vcl::timer_interval timer, vcl::buffer<vcl::vec3>& points);

    //void keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods);
    gui_bonhomme_structure gui_bon;

    vcl::hierarchy_mesh_drawable hierarchy;
    vcl::hierarchy_mesh_drawable_display_skeleton hierarchy_visual_debug;

    vcl::vec2 position = {0,0};
    float rotation = 0.0f;

};


