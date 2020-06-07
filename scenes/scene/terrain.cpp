
#include "terrain.hpp"
//#include "terrain_creation.hpp"
//#include "marching_cubes.hpp"


// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;

mesh create_grid(const gui_scene_structure& gui_scene);

float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v);
mesh create_terrain();


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{

    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

    compute_query();    

    terraingen.GenerateBuffer();

    opengl_debug();
    terraingen.GenerateMesh();

    opengl_debug();

    terrain = mesh_drawable(terraingen.generator_mesh);

    //terrain.uniform.color = {1.f,1.f,1.f}; // 0.6f, 0.85f, 0.5f
    terrain.uniform.color = {1.f,.0f,.0f}; // 0.6f, 0.85f, 0.5f
    terrain.uniform.shading.specular = 0.0f; // non-specular terrain material

    //terrain_texture = create_texture_gpu( image_load_png("scenes/3D_graphics/02_texture/assets/grass.png") );

    counter = 0;

}




/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    /*
    ++counter;
    if (counter == 60) {
        counter = 0;
        std::cout<<"Iterations : "<<terraingen.numAnimatedErosionIterations<<std::endl;
    }
    */
    set_gui();

    //terraingen.UpdateErosion();
    //terrain.update_position(terraingen.mesh.position);
    //terrain.update_normal(terraingen.mesh.normal);

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe

    // Display terrain
    glPolygonOffset( 1.0, 4.0 );

    // Before displaying a textured surface: bind the associated texture id
    //glBindTexture(GL_TEXTURE_2D, terrain_texture);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if( gui_scene.surface ){ // surface if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        draw(terrain, scene.camera, shaders["mesh_fixed"]);
    }
    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        draw(terrain, scene.camera, shaders["wireframe"]);
    }
    //glBindTexture(GL_TEXTURE_2D, scene.texture_white);

}


// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_z(float u, float v)
{
    float z = 0;

    const size_t N = 4;
    std::array<vec2, N> p_list = {vec2(0.f,0.f),vec2(0.5f,0.5f),vec2(0.2f,0.7f),vec2(0.8f,0.7f)};
    std::array<float, N> h_list = {3.f,-1.5f,1.f,2.f};
    std::array<float, N> sigma_list = {0.5f,0.15f,0.2f,0.2f};

    for (size_t i=0; i<N; i++)
    {
        const float di = norm(vec2(u,v)-p_list[i])/sigma_list[i];
        z += h_list[i] * std::exp(-di*di);
    }

    const float scaling = 3.0f;
    const int octave = 6;
    const float persistency = 0.4f;

    // Evaluate Perlin noise
    const float noise = perlin(scaling*u, scaling*v, octave, persistency);

    z += 0.3f * noise;

    return z;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v)
{
    const float x = 20*(u-0.5f);
    const float y = 20*(v-0.5f);
    const float z = evaluate_terrain_z(u,v);

    return {x,y,z};
}

// Generate terrain mesh
mesh create_terrain()
{
    // Number of samples of the terrain is N x N
    const size_t N = 100;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.texture_uv.resize(N*N);

    // Fill terrain geometry
    for(size_t ku=0; ku<N; ++ku)
    {
        for(size_t kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            // Compute coordinates
            vec3 pos = evaluate_terrain(u,v);

            terrain.position[kv+N*ku] = pos;

            terrain.texture_uv[kv+N*ku] = vec2(ku/5.f,kv/5.f);

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

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    return terrain;
}


void scene_model::set_gui()
{
    ImGui::Text("Scene: "); ImGui::SameLine();
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface);     ImGui::SameLine();
}



