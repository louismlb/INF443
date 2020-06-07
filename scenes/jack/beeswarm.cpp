#include "beeswarm.hpp"

using namespace vcl;

void beeswarm::create_ruche(float size)
{
    ruche = mesh_primitive_sphere(size);
    ruche.uniform.color = {0.87f, 0.69f, 0.17f};
    ruche.uniform.shading.specular = 0;
}

void beeswarm::create_bee(float size)
{
    mesh _body = mesh_primitive_sphere(size);
    for (int i=0; i<_body.texture_uv.size(); i++)
    {
        float x = _body.position[i][0];
        float y = _body.position[i][1];
        _body.texture_uv[i] = {-y/(2*size) + 0.5f, -x/(2*size) + 0.5f};
    }
    mesh_drawable body = _body;
    body.uniform.transform.scaling_axis = {1.5,1,1};
    body.uniform.color = {1,1,0};
    body.texture_id = create_texture_gpu( image_load_png("scenes/shared_assets/textures/abeille.png") );
    mesh_drawable head = mesh_drawable( mesh_primitive_sphere(size/1.5f));
    head.uniform.color = {0,0,0};

    mesh _wing;
    float theta = 2*M_PI/20;
    _wing.position.push_back({0,0,0});
    for(int i=0; i<13; i++)
    {
        _wing.position.push_back(size*vec3(std::cos(theta*(i-2)), std::sin(theta*(i-2)), 0));
        _wing.connectivity.push_back({i+1, i+2, 0});
    }
    _wing.position.push_back(size*vec3(std::cos(theta*12), std::sin(theta*(12)), 0));
    _wing.connectivity.push_back({14, 1, 0});

    mesh_drawable wing = _wing;
    wing.uniform.transform.scaling_axis = {1,2,1};
    wing.uniform.color = {0.2f,1,1};

    bee.add(body, "body");
    bee.add(head, "head", "body", {1.7f*size,0,0});
    bee.add(wing, "left_wing", "body", {0, 0.5f*size, 0});
    bee.add(wing, "right_wing", "body", {{0, -0.5f*size, 0}, {1,0,0, 0,-1,0, 0,0,1}});
}



void beeswarm::setup_swarm(std::map<std::string,GLuint>& shaders, scene_structure& scene)
{
    create_ruche(0.5f);
    create_bee(0.05f);
    pos_bees = {{0,0,7}, {0,0,7}, {0,0,7}, {0,0,7}, {0,0,7}, {0,0,7}, {0,0,7}, {0,0,7}, {0,0,7}};
    v_bees = {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}};

    bee["left_wing"].element.texture_id = scene.texture_white;
    bee.set_shader_for_all_elements(shaders["mesh"]);
}

void beeswarm::draw_swarm(std::map<std::string,GLuint>& shaders, scene_structure& scene, vcl::timer_interval timer)
{
    update_swarm(timer);

    if(gui_bee.surface)
    {
        ruche.uniform.transform.translation = {0,0,7};
        ruche.uniform.transform.scaling_axis = {1,1,0.4f};
        draw(ruche, scene.camera, shaders["mesh"]);
        ruche.uniform.transform.scaling_axis = {0.85f, 0.85f, 0.4f};
        ruche.uniform.transform.translation = {0,0,7.15f};
        draw(ruche, scene.camera, shaders["mesh"]);
        ruche.uniform.transform.translation = {0,0,6.85f};
        draw(ruche, scene.camera, shaders["mesh"]);
        ruche.uniform.transform.scaling_axis = {0.6f, 0.6f, 0.3f};
        ruche.uniform.transform.translation = {0,0,6.7f};
        draw(ruche, scene.camera, shaders["mesh"]);
    }
    if(gui_bee.wireframe)
    {
        ruche.uniform.transform.translation = {0,0,7};
        ruche.uniform.transform.scaling_axis = {1,1,0.4f};
        draw(ruche, scene.camera, shaders["wireframe"]);
        ruche.uniform.transform.scaling_axis = {0.85f, 0.85f, 0.4f};
        ruche.uniform.transform.translation = {0,0,7.15f};
        draw(ruche, scene.camera, shaders["wireframe"]);
        ruche.uniform.transform.translation = {0,0,6.85f};
        draw(ruche, scene.camera, shaders["wireframe"]);
        ruche.uniform.transform.scaling_axis = {0.6f, 0.6f, 0.3f};
        ruche.uniform.transform.translation = {0,0,6.7f};
        draw(ruche, scene.camera, shaders["wireframe"]);
    }

    mat3 const R_y_sym = {1,0,0, 0,-1,0, 0,0,1};

    bee["left_wing"].transform.rotation = rotation_from_axis_angle_mat3({1,0,0}, 0.8f*std::sin(8*M_PI*timer.t));
    bee["right_wing"].transform.rotation = R_y_sym*rotation_from_axis_angle_mat3({1,0,0}, 0.8f*std::sin(8*M_PI*timer.t));
    for(std::vector<vec3>::iterator ptr = pos_bees.begin()+1; ptr < pos_bees.end(); ptr++)
    {
        bee["body"].transform.translation = *ptr;
        bee["body"].transform.rotation = rotation_between_vector_mat3({1,0,0}, v_bees[ptr-pos_bees.begin()]);
        bee.update_local_to_global_coordinates();

        if(gui_bee.surface)
            draw(bee, scene.camera);
        if(gui_bee.wireframe)
            draw(bee, scene.camera, shaders["wireframe"]);
    }
}

void beeswarm::update_swarm(vcl::timer_interval timer)
{
    float m = 0.1f;
    float dt = 0.1f;
    float K = 0.7f;
    float K2 = 0.2f;
    float L0 = 2.0f;
    float L1 = 1.0f;

    float speed0 = norm(*target - pos_bees[0]);
    if (speed0 < 0.2f)
    {
        v_bees[0] = *target - pos_bees[0];
        K = 0.2f;
        K2 = 0.2f;
        L0 = 2.0f;
        L1 = 1.0f;
    }

    else
    {
        v_bees[0] = speed0*(*target - pos_bees[0]);
        K = 0.7f;
        K2 = 0.2f;
        L0 = 0.7f;
        L1 = 0.5f;
    }

    for (int i=1; i<9; i++)
    {
        v_bees[i] += interaction_force(pos_bees[i], pos_bees[0], L1, K2)*dt/m;
        for (int j=1; j<9; j++)
        {
            if(j!=i)
                v_bees[i] += interaction_force(pos_bees[i], pos_bees[j], L0, K)*dt/m;
        }
    }

    pos_bees[0] += v_bees[0]*dt;
    for (int i=1; i<9; i++)
    {
        float speed = norm(v_bees[i]);
        if (i!=0)
        {
            if (speed0 < 0.1f)
                v_bees[i] /= 3*speed;
            else
                v_bees[i] /= speed;
        }

        //Si à l'arret
        vec3 v = pos_bees[i]-pos_bees[0];
        vec3 v2 = vec3(v[1], -v[0], 0);
        v_bees[i] += 0.015f * normalize(v2) + vec3(rand_interval(-0.1f,0.1f), rand_interval(-0.1f,0.1f), rand_interval(-0.1f,0.1f));
        pos_bees[i] += v_bees[i]*dt;

        v = pos_bees[i]-pos_bees[0];
        v[2] = 0;

        //Si dans le sol
        float b = (environment->height(pos_bees[i][0], pos_bees[i][1]))[2] + 0.4f;
        if(pos_bees[i][2] < b)
            pos_bees[i][2] = b;
    }
}

void gui_beeswarm_structure::set_gui_bees(bool surf, bool wire)
{
    surface = surf;
    wireframe = wire;
}



vec3 interaction_force(const vec3& pi, const vec3& pj, float L0, float K)
{
    float L = norm(pi-pj);
    return (-K*(L-L0))*normalize(pi-pj);

}
