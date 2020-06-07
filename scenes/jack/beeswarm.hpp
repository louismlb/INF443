#pragma once
#include "main/scene_base/base.hpp"
#include "terrain.hpp"

struct gui_beeswarm_structure
{
    bool surface = true;
    bool wireframe = false;

    void set_gui_bees(bool surface, bool wireframe);
};

struct beeswarm
{
    vcl::mesh_drawable ruche;
    vcl::hierarchy_mesh_drawable bee;
    vcl::hierarchy_mesh_drawable swarm;

    gui_beeswarm_structure gui_bee;

    std::vector<vcl::vec3> pos_bees;
    std::vector<vcl::vec3> v_bees;

    vcl::vec3* target;
    terrain* environment;

    void setup_swarm(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_swarm(std::map<std::string,GLuint>& shaders, scene_structure& scene, vcl::timer_interval timer);

    void update_swarm(vcl::timer_interval timer);

    void create_ruche(float size);
    void create_bee(float size);
    void set_hierarchy(int N);
};

vcl::vec3 interaction_force(const vcl::vec3& pi, const vcl::vec3& pj, float L0, float K);
