#include <fstream>
#include <sstream>
#include "main/scene_base/base.hpp"
#include "terrain.hpp"



using namespace vcl;



// Creation functions
void terrain::create_ground()
{
    // Number of samples of the terrain is N x N
    const size_t N = 100;
    mesh m_ground;
    //float c;

    m_ground.position.resize(N*N);
    points.resize(N*N);
    //ground.color.resize(N*N);
    m_ground.texture_uv.resize(N*N);

    // Fill terrain geometry
    for(size_t ku=0; ku<N; ++ku)
    {
        for(size_t kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            // Compute coordinates
            //c = 0.5f+0.6f*perlin(1.7f*u, 1.7f*v, 9, 0.56f);

            //terrain.color[kv+N*ku]  = {c,c,c,1.0f};
            m_ground.position[kv+N*ku] = evaluate_ground(u,v);
            points[kv+N*ku] = evaluate_ground(u,v);
            m_ground.texture_uv[kv+N*ku] = {v, u};
        }
    }


    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    const unsigned int Ns = N;
    for(unsigned int ku=0; ku<Ns-1; ++ku)
    {
        for(unsigned int kv=0; kv<Ns-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+Ns, idx+1};
            const uint3 triangle_2 = {idx, idx+Ns, idx+1+Ns};

            m_ground.connectivity.push_back(triangle_1);
            m_ground.connectivity.push_back(triangle_2);
        }
    }
    ground = m_ground;
    //ground = mesh_primitive_quad({-20,-20,0}, {20,-20,0}, {20,20,0}, {-20,20,0});
}

void terrain::create_sky() {

    mesh m_sky;
    m_sky.position = {{-50,-50,-50},{-50,-50,50},{-50,50,-50},{-50, 50, 50},{50,50,-50},{50,50,50},{50,-50,-50},{50,-50,50},  {50,50,-50},{50,50,50},{50,-50,-50},{50,-50,50},  {50,-50,-50},{50,-50,50}};
    m_sky.connectivity = {{0,2,1},{1,2,3},{2,4,3},{3,4,5},{4,6,5},{5,6,7},{12,0,13},{13,0,1},  {1,3,9},{1,9,11}, {0,8,2},{0,10,8}};
    m_sky.texture_uv = {{0.25f,0.65},{0.25f,0.34},{0.5f,0.65},{0.5f,0.34},{0.75f,0.65},{0.75f,0.34},{1,0.65},{1,0.34}, {0.5f,1},{0.5f,0},{0.25f,1},{0.25f,0},  {0,0.65},{0,0.34}};
    sky = m_sky;
    sky.uniform.shading = {1,0,0};
}

void terrain::create_sea(float height)
{
    int N = 100;
    mesh m_sea;
    m_sea.position.resize(N*N);
    m_sea.texture_uv.resize(N*N);
    m_sea.color.resize(N*N);

    for(int ku=0; ku<N; ku++)
    {
        for(int kv=0; kv<N; kv++)
        {
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);
            m_sea.position[kv+N*ku] = {40*(u-0.5f), 40*(v-0.5f), 0.4f*perlin(5*u, 5*v, 7, height + 0.66f)};
            m_sea.texture_uv[kv+N*ku] = {4*u, 4*v};

            if(gui_ter.ground)
            {
                float h = points[kv + N*ku][2];
                if (h > 1.0f)
                    h = 2.0f-h;
                if (h > 0.0f)
                {
                    float coef = h;
                    vec4 c = (1-coef) * vec4(1, 1, 1, 1);
                    c += coef * (perlin(100*u, 100*v, 5, 1.0f) - 1.0f) * vec4(0.7f, 0.7f, 0.7f, 1.0f);

                    m_sea.color[kv+N*ku] = c;
                    m_sea.position[kv + N*ku][2] += 0.3f* coef * perlin(100*u, 100*v, 5, 0.5f);
                }
                else if (h > -0.6f)
                {
                    float c, a;
                    a = (0.6f + h)/0.55f;
                    c = std::exp(h+0.2f);
                    m_sea.color[kv+N*ku] = {c, 0.6f + 0.4f*c, 0.8f+0.2f*c, 0.6f + 0.4f*a};
                }
                else
                {
                    float c, a;
                    c = std::exp(h+0.1f);
                    a = std::pow(std::abs(h+0.1f)/3.0f, 1.3f);
                    m_sea.color[kv+N*ku] = {c, 0.6f + 0.4f*c, 0.8f+0.2f*c, 0.6f + 0.4f*a};
                }
            }
            else
                m_sea.color[kv+N*ku] = {0, 0.6f, 0.8f, 0.8f};
        }
    }

    for(int ku=0; ku<N-1; ++ku)
    {
        for(int kv=0; kv<N-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+N, idx+1};
            const uint3 triangle_2 = {idx, idx+N, idx+1+N};

            m_sea.connectivity.push_back(triangle_1);
            m_sea.connectivity.push_back(triangle_2);
        }
    }
    sea = m_sea;
}



void terrain::setup_terrain(scene_structure& scene)
{
    create_ground();
    create_sea(0.0f);
    create_sky();
    create_tree(0.15f, 5.0f);
    create_grass(0.5f);
    create_algue(2.0f);
    create_bush(3.0f);
    create_branche(3.0f);

    ground.uniform.shading.specular = 0.0f;

    tree.texture_id = scene.texture_white;

    texture_id_ground = create_texture_gpu( image_load_png("scenes/shared_assets/textures/island2.png") );
    texture_id_sky = create_texture_gpu( image_load_png("scenes/shared_assets/textures/skybox.png") );
    texture_id_sea = scene.texture_white;//create_texture_gpu( image_load_png("scenes/shared_assets/textures/sea.png") );
    texture_id_grass = create_texture_gpu( image_load_png("scenes/shared_assets/textures/billboard_grass.png") );
    texture_id_algue = create_texture_gpu( image_load_png("scenes/shared_assets/textures/algue.png") );
    texture_id_bush = create_texture_gpu( image_load_png("scenes/shared_assets/textures/paradis.png") );
    texture_id_trunk_palmtree = create_texture_gpu( image_load_png("scenes/shared_assets/textures/tronc_palmier.png"), GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT );
    texture_id_leaves_palmtree = create_texture_gpu( image_load_png("scenes/shared_assets/textures/palm_leaves.png") );

    tree.texture_id = texture_id_trunk_palmtree;
    branche.texture_id = texture_id_leaves_palmtree;

    vegetation_position.resize(4);
    vegetation_scaling.resize(5);
    load_vegetation();
}

void terrain::draw_terrain(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_terrain_structure&, float time)
{

    if ((gui_ter.ground == false && ground_is_here == true) || (gui_ter.ground == true && ground_is_here == false))
    {
        create_sea(0.0f);
        ground_is_here = !ground_is_here;
    }


        //Display ground
        if(gui_ter.ground)
        {
            if(gui_ter.surface)
                draw(ground, scene.camera, shaders["mesh"], texture_id_ground);
            if(gui_ter.wireframe)
                draw(ground, scene.camera, shaders["wireframe"], texture_id_ground);
        }


        //Display sky
        if(gui_ter.surface && gui_ter.sky)
            draw(sky, scene.camera, shaders["mesh"], texture_id_sky);



        //Display leaes of palmtrees
        if(gui_ter.tree)
        {
            for(std::vector<vec3>::iterator ptr = vegetation_position[0].begin(); ptr < vegetation_position[0].end(); ptr++)
            {
                float l_or = palmtrees_orientation[ptr-vegetation_position[0].begin()];
                float t_sc = vegetation_scaling[0][ptr-vegetation_position[0].begin()];


                tree.uniform.transform.translation = *ptr;
                tree.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, l_or);
                tree.uniform.transform.scaling = t_sc;

                //Display trunk
                if(gui_ter.surface)
                    draw(tree, scene.camera, shaders["mesh"]);
                if(gui_ter.wireframe)
                    draw(tree, scene.camera, shaders["wireframe"]);
            }
        }

        //Billboards
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(false);

        //Display seaweed
        if(gui_ter.algue)
        {
            for(std::vector<vec3>::iterator ptr = vegetation_position[2].begin(); ptr < vegetation_position[2].end(); ptr++) {

                algue.uniform.transform.translation = *ptr - vec3(0.2f,0,-0.3f);
                algue.uniform.transform.rotation = scene.camera.orientation;
                algue.uniform.transform.scaling = vegetation_scaling[2][ptr-vegetation_position[2].begin()];

                if(gui_ter.surface)
                    draw(algue, scene.camera, shaders["mesh"], texture_id_algue);
                if(gui_ter.wireframe)
                    draw(algue, scene.camera, shaders["wireframe"]);


            }
        }

        glDepthMask(true);
        //Display sea
        if(gui_ter.sea)
        {
            if(gui_ter.surface)
                draw_sea(sea, scene.camera, shaders["sea"], time);
            if(gui_ter.wireframe)
                draw_sea(sea, scene.camera, shaders["sea_wireframe"], time);
        }

        glDepthMask(false);
        //Display bushes
        if(gui_ter.bush)
        {
            for(std::vector<vec3>::iterator ptr = vegetation_position[3].begin(); ptr < vegetation_position[3].end(); ptr++) {

                bush.uniform.transform.translation = *ptr - vec3(0.2f,0,-0.3f);
                bush.uniform.transform.scaling = vegetation_scaling[3][ptr-vegetation_position[3].begin()];

                if(gui_ter.surface)
                {
                    bush.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, bushes_orientation[ptr-vegetation_position[3].begin()]);
                    draw(bush, scene.camera, shaders["mesh"], texture_id_bush);
                    bush.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, M_PI/3 + bushes_orientation[ptr-vegetation_position[3].begin()]);
                    draw(bush, scene.camera, shaders["mesh"], texture_id_bush);
                    bush.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, 2*M_PI/3 + bushes_orientation[ptr-vegetation_position[3].begin()]);
                    draw(bush, scene.camera, shaders["mesh"], texture_id_bush);
                }
                if(gui_ter.wireframe)
                {
                    bush.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, bushes_orientation[ptr-vegetation_position[3].begin()]);
                    draw(bush, scene.camera, shaders["wireframe"]);
                    bush.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, M_PI*bushes_orientation[ptr-vegetation_position[3].begin()]/2);
                    draw(bush, scene.camera, shaders["wireframe"]);
                    bush.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, 2*M_PI/3 + bushes_orientation[ptr-vegetation_position[3].begin()]);
                    draw(bush, scene.camera, shaders["wireframe"]);
                }


            }
        }

        //Display grass
        if(gui_ter.grass)
        {
            for(std::vector<vec3>::iterator ptr = vegetation_position[1].begin(); ptr < vegetation_position[1].end(); ptr++) {

                grass.uniform.transform.translation = *ptr - vec3(0.2f,0,-0.3f);
                grass.uniform.transform.rotation = scene.camera.orientation;
                grass.uniform.transform.scaling = vegetation_scaling[1][ptr-vegetation_position[1].begin()];

                if(gui_ter.surface)
                    draw(grass, scene.camera, shaders["mesh"], texture_id_grass);
                if(gui_ter.wireframe)
                    draw(grass, scene.camera, shaders["wireframe"]);


            }
        }

        //Display leaes of palmtrees
        if(gui_ter.tree)
        {
            mat3 M = rotation_from_axis_angle_mat3({1,0,0}, -2)*rotation_from_axis_angle_mat3({0,0,1}, M_PI/4);
            for(std::vector<vec3>::iterator ptr = vegetation_position[0].begin(); ptr < vegetation_position[0].end(); ptr++)
            {
                float l_or = palmtrees_orientation[ptr-vegetation_position[0].begin()];
                float t_sc = vegetation_scaling[0][ptr-vegetation_position[0].begin()];


                tree.uniform.transform.translation = *ptr;
                tree.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, l_or);
                tree.uniform.transform.scaling = t_sc;

                branche.uniform.transform.translation = *ptr + t_sc * vec3((0.8f*std::log(12)-0.5f)*std::cos(l_or), (0.8f*std::log(12)-0.5f)*std::sin(l_or), 4.8f);
                branche.uniform.transform.scaling = t_sc;
                branche.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, l_or)*M;

                glDepthMask(true);
                //Display trunk
                if(gui_ter.surface)
                    draw(tree, scene.camera, shaders["mesh"]);
                if(gui_ter.wireframe)
                    draw(tree, scene.camera, shaders["wireframe"]);
                glDepthMask(false);

                //Display leaves
                if(gui_ter.surface)
                {
                    draw(branche, scene.camera, shaders["mesh"]);
                    branche.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, M_PI/2 + l_or)*M;
                    draw(branche, scene.camera, shaders["mesh"]);
                    branche.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, M_PI + l_or)*M;
                    draw(branche, scene.camera, shaders["mesh"]);
                    branche.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, 3*M_PI/2 + l_or)*M;
                    draw(branche, scene.camera, shaders["mesh"]);
                }
                if(gui_ter.wireframe)
                {
                    draw(branche, scene.camera, shaders["wireframe"]);
                    branche.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, M_PI/2 + l_or)*M;
                    draw(branche, scene.camera, shaders["wireframe"]);
                    branche.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, M_PI + l_or)*M;
                    draw(branche, scene.camera, shaders["wireframe"]);
                    branche.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, 3*M_PI/2 + l_or)*M;
                    draw(branche, scene.camera, shaders["wireframe"]);
                }
            }
        }

        glDepthMask(true);





}

void terrain::draw_sea(const mesh_drawable& drawable, const camera_scene& camera, const GLuint shader, float t)
{

    // Switch shader program only if necessary
    GLint current_shader = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader); opengl_debug();
    if(shader!=GLuint(current_shader))
        glUseProgram(shader); opengl_debug();

     glBindTexture(GL_TEXTURE_2D, texture_id_sea);  opengl_debug();
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glDepthMask(false);


    // Send all uniform values to the shader
    uniform(shader, "color_alpha", drawable.uniform.color_alpha);                opengl_debug();
    uniform(shader, "color", drawable.uniform.color);                opengl_debug();


    uniform(shader,"perspective",camera.perspective.matrix());         opengl_debug();
    uniform(shader,"view",camera.view_matrix());                       opengl_debug();
    uniform(shader,"camera_position",camera.camera_position());        opengl_debug();

    uniform(shader, "ambiant", drawable.uniform.shading.ambiant);      opengl_debug();
    uniform(shader, "diffuse", drawable.uniform.shading.diffuse);      opengl_debug();
    uniform(shader, "specular", drawable.uniform.shading.specular);    opengl_debug();
    uniform(shader, "specular_exponent", drawable.uniform.shading.specular_exponent); opengl_debug();
    uniform(shader, "time", t);

    vcl::draw(drawable.data); opengl_debug();
    glDepthMask(true);
}










// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluate_ground_z(float u, float v){

    const vec2 u0[4] = {{0.4f, 0.4f},{0.5f, 0.6f},{0.4f, 0.7f}, {0.8f, 0.2f}};
    const float h0[4] = {3.0f, -1.5f, 1.0f, 2.0f};
    const float sigma0[4] = {0.5f, 0.15f, 0.2f, 0.2f};

    float d = 0.0f;
    float z = 0.0f;

    // Evaluate Perlin noise
    const float noise1 = 0.4f*perlin(1.7f*u, 1.7f*v, 9, 0.56f);
    const float noise2 = perlin(2.7f*u, 2.7f*v, 2, 0.56f);
    const float noise3 = perlin(1.7f*u, 1.7f*v, 1, 0.56f);

    for(int i=0; i<4; i++){
         d = norm(vec2(u,v)-u0[i])/sigma0[i];
         z += h0[i]*std::exp(-d*d);
    }
    //return z + 0.1f*noise*std::abs(z-3.0f);
    return z-2.5f + noise3 + noise1*std::abs(z-1.5f);// + noise2;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_ground(float u, float v)
{
    const float x = 20*(u-0.5f);
    const float y = 20*(v-0.5f);
    const float z = evaluate_ground_z(u,v);

    return {2*x,2*y,3*z};
}



vec3 terrain::height(float x, float y)
{
    //int N = std::sqrt(points.size());
    const size_t N = 100;
    int ku = std::floor(N*(x/40 +0.5f));
    int kv = std::floor(N*(y/40 +0.5f));

    if (kv + N*ku < N*N)
        return points[kv + N*ku];
    else
        return vec3(x,y,0);
}


void gui_terrain_structure::set_gui_terrain(bool surf, bool wire, std::vector<std::vector<vcl::vec3>>& vegetation_position)
{
    surface = surf;
    wireframe = wire;

    ImGui::Checkbox("Ground", &ground);
    ImGui::SameLine();
    ImGui::Checkbox("Sky", &sky);
    ImGui::SameLine();
    ImGui::Checkbox("Sea", &sea);
    ImGui::SameLine();
    ImGui::Checkbox("Trees", &tree);
    ImGui::SameLine();
    ImGui::Checkbox("Grass", &grass);
    ImGui::SameLine();
    ImGui::Checkbox("Seaweed", &algue);
    ImGui::SameLine();
    ImGui::Checkbox("Bushes", &bush);
    ImGui::Spacing();

    const char* items[4] = {"Tree", "Grass", "Seaweed", "Bush"};
    //veg_ind = 0;
    //static int item_current = 0;
    ImGui::Text("Vegetation : ");
    ImGui::SameLine();
    ImGui::ListBox("    ", &veg_ind, items, IM_ARRAYSIZE(items));
    ImGui::SameLine();
    if(ImGui::Button("Remove vegetation"))
    {
        if (vegetation_position[veg_ind].size())
        {
            vegetation_position[veg_ind].pop_back();
        }
    }
    ImGui::Spacing();
    if(ImGui::Button("Save Vegetation Configuration"))
    {
        save_vegetation(vegetation_position);
    }
    ImGui::Spacing();
}
