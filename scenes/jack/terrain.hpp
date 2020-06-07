#pragma once
#include "main/scene_base/base.hpp"

struct gui_terrain_structure
{
    bool surface = true;
    bool wireframe = false;
    static int veg_ind;

    bool sea = true;
    bool ground =true;
    bool sky = true;
    bool tree = true;
    bool grass = true;
    bool algue = true;
    bool bush = true;


    void set_gui_terrain(bool surf, bool wire, std::vector<std::vector<vcl::vec3>>& vegetation_position);
};



struct terrain
{
    //Structures élémentaires de l'environnement
    vcl::mesh_drawable ground;
    vcl::mesh_drawable sky;
    vcl::mesh_drawable sea;
    bool ground_is_here = true;

    //Structures de la végétation
    vcl::mesh_drawable tree;
    vcl::mesh_drawable grass;
    vcl::mesh_drawable algue;
    vcl::mesh_drawable bush;
    vcl::mesh_drawable branche;

    //std::vector<vcl::vec3> tree_position;
    std::vector<std::vector<vcl::vec3>> vegetation_position;
    std::vector<std::vector<float>> vegetation_scaling;

    std::vector<float> palmtrees_orientation;
    std::vector<float> bushes_orientation;

    vcl::buffer<vcl::vec3> points;   //Pas forcément utile

    GLuint texture_id_ground;
    GLuint texture_id_sky;
    GLuint texture_id_sea;
    GLuint texture_id_grass;
    GLuint texture_id_algue;
    GLuint texture_id_trunk_palmtree;
    GLuint texture_id_bush;
    GLuint texture_id_leaves_palmtree;

    gui_terrain_structure gui_ter;


    void create_ground();
    void create_sky();
    void create_sea(float height = 0.0f);

    void create_tree(float radius, float height);
    void create_grass(float size);
    void create_algue(float size);
    void create_bush(float size);
    void create_branche(float size);


    void mouse_click(scene_structure& scene, GLFWwindow* window, int , int , int );

    vcl::vec3 height(float x, float y);


    void load_vegetation();
    void setup_terrain(scene_structure& scene);
    void draw_terrain(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_terrain_structure& gui, float time);
    void draw_sea(const vcl::mesh_drawable& sea, const vcl::camera_scene& camera, GLuint shader, float t);


};

float evaluate_ground_z(float u, float v);
vcl::vec3 evaluate_ground(float u, float v);
void save_vegetation(std::vector<std::vector<vcl::vec3>>& vegetation_position);
