#include <fstream>
#include <sstream>
#include "main/scene_base/base.hpp"
#include "terrain.hpp"

using namespace vcl;

int gui_terrain_structure::veg_ind = 0;


//Creation functions
void terrain::create_tree(float radius, float height)
{
    int N = 10;
    int M = 10;
    const float dh = height/M;
    const float theta = 2*3.1415926f/N;
    mesh m_tree;
    m_tree.position.resize(M*(N+1));
    m_tree.texture_uv.resize(M*(N+1));

    for(int i = 0; i<M; i++)
    {
        for(int j = 0; j<=N; j++)
        {
            m_tree.position[i*(N+1) + j] = {radius*std::cos(theta*j) + 0.8f*std::log(2+i+std::cos(theta*j)/(i+1)) - 0.5f, radius*(1 + 2/(i+1))*std::sin(theta*j), i*dh};
            m_tree.texture_uv[i*(N+1) + j] = {i*2.0f, j/5.0f};
        }
    }

    for(int i = 0; i<M-1; i++)
    {
        for(int j = 0; j<N; j++)
        {
            const unsigned int idx = i*(N+1) + j; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1, idx+N+2};
            const uint3 triangle_2 = {idx, idx+N+2, idx+1+N};

            m_tree.connectivity.push_back(triangle_1);
            m_tree.connectivity.push_back(triangle_2);
        }
    }

    tree = m_tree;
    //tree.uniform.color = {0.3f,0.2f,0.2f};
    tree.uniform.shading.specular = 0.0f;
    tree.texture_id = texture_id_trunk_palmtree;

}

void terrain::create_grass(float size)
{
    grass = mesh_primitive_quad( {size,size,0}, {0, size,0}, {0,0,0}, {size,0,0});
    grass.uniform.shading = {1,0,0};
}

void terrain::create_algue(float size)
{
    algue = mesh_primitive_quad( {size,size,0}, {0, size,0}, {0,0,0}, {size,0,0});
    algue.uniform.shading = {1,0,0};
}

void terrain::create_bush(float size)
{
    bush = mesh_primitive_quad( {size/2,0,size/2}, {-size/2,0, size/2}, {-size/2,0,-size/2}, {size/2,0,-size/2});
    bush.uniform.shading = {1,0,0};
}


void terrain::create_branche(float size)
{
    mesh _branche;
    _branche.position = {{0,0,0}, {0,0,size}, {0,size/5,size}, {0, size/5,0}, {0,0,0}, {0,0,size}, {size/5,0,size}, {size/5,0,0}};
    _branche.connectivity = {{0,1,2}, {0,2,3}, {4,6,5}, {4,7,6}};
    _branche.texture_uv = {{1,1}, {1,0}, {0,0}, {0,1}, {1,1}, {1,0}, {0,0}, {0,1}};

    branche = _branche;
    branche.uniform.shading = {1,0,0};
}







//Save and Load vegetation functions
void save_vegetation(std::vector<std::vector<vcl::vec3>>& vegetation_position)
{
    std::ofstream treestream("scenes/shared_assets/trees");
    std::ofstream grasstream("scenes/shared_assets/grass");
    std::ofstream seaweedstream("scenes/shared_assets/seaweed");
    std::ofstream bushestream("scenes/shared_assets/bushes");

    for (std::vector<vec3>::iterator ptr = vegetation_position[0].begin(); ptr < vegetation_position[0].end(); ptr++)
    {
        treestream << (*ptr)[0] << " " << (*ptr)[1] << " " << (*ptr)[2] << std::endl;
    }
    for (std::vector<vec3>::iterator ptr = vegetation_position[1].begin(); ptr < vegetation_position[1].end(); ptr++)
    {
        grasstream << (*ptr)[0] << " " << (*ptr)[1] << " " << (*ptr)[2] << std::endl;
    }
    for (std::vector<vec3>::iterator ptr = vegetation_position[2].begin(); ptr < vegetation_position[2].end(); ptr++)
    {
        seaweedstream << (*ptr)[0] << " " << (*ptr)[1] << " " << (*ptr)[2] << std::endl;
    }
    for (std::vector<vec3>::iterator ptr = vegetation_position[3].begin(); ptr < vegetation_position[3].end(); ptr++)
    {
        bushestream << (*ptr)[0] << " " << (*ptr)[1] << " " << (*ptr)[2] << std::endl;
    }
}

void terrain::load_vegetation()
{
    std::ifstream treestream("scenes/shared_assets/trees");
    std::ifstream grasstream("scenes/shared_assets/grass");
    std::ifstream seaweedstream("scenes/shared_assets/seaweed");
    std::ifstream bushestream("scenes/shared_assets/bushes");

    std::string line;

    while(getline(treestream, line))
    {
        std::istringstream iss(line);
        std::string x_str, y_str, z_str;
        float x, y, z;

        getline(iss, x_str, ' ');
        getline(iss, y_str, ' ');
        getline(iss, z_str);
        x = std::stof(x_str);
        y = std::stof(y_str);
        z = std::stof(z_str);

        vegetation_position[0].push_back(vec3(x, y, z));
        palmtrees_orientation.push_back(rand_interval(0.0f, 2*M_PI));
        vegetation_scaling[0].push_back(rand_interval(0.7f,1.8f));
    }

    while(getline(grasstream, line))
    {
        std::istringstream iss(line);
        std::string x_str, y_str, z_str;
        float x, y, z;

        getline(iss, x_str, ' ');
        getline(iss, y_str, ' ');
        getline(iss, z_str);
        x = std::stof(x_str);
        y = std::stof(y_str);
        z = std::stof(z_str);

        vegetation_position[1].push_back(vec3(x, y, z));
        vegetation_scaling[1].push_back(rand_interval(1.0f,2.0f));
    }

    while(getline(seaweedstream, line))
    {
        std::istringstream iss(line);
        std::string x_str, y_str, z_str;
        float x, y, z;

        getline(iss, x_str, ' ');
        getline(iss, y_str, ' ');
        getline(iss, z_str);
        x = std::stof(x_str);
        y = std::stof(y_str);
        z = std::stof(z_str);

        vegetation_position[2].push_back(vec3(x, y, z));
        vegetation_scaling[2].push_back(rand_interval(0.5f,1.0f));
    }

    while(getline(bushestream, line))
    {
        std::istringstream iss(line);
        std::string x_str, y_str, z_str;
        float x, y, z;

        getline(iss, x_str, ' ');
        getline(iss, y_str, ' ');
        getline(iss, z_str);
        x = std::stof(x_str);
        y = std::stof(y_str);
        z = std::stof(z_str);

        vegetation_position[3].push_back(vec3(x, y, z));
        bushes_orientation.push_back(rand_interval(0.0f, 2*M_PI));
        vegetation_scaling[3].push_back(rand_interval(0.5f,1.2f));
    }



}

//Add vegetation when shift + left click
void terrain::mouse_click(scene_structure& scene, GLFWwindow* window, int , int , int )
{
    const bool mouse_click_left  = glfw_mouse_pressed_left(window);
    const bool key_shift = glfw_key_shift_pressed(window);

    if(mouse_click_left && key_shift)
    {
        // Get vector orthogonal to camera orientation
        const mat4 M = scene.camera.camera_matrix();
        const vec3 n = {M(0,2),M(1,2),M(2,2)};

        // Compute intersection between current ray and the plane orthogonal to the view direction and passing by the selected object
        const vec2 cursor = glfw_cursor_coordinates_window(window);
        const ray r = picking_ray(scene.camera, cursor);
        //vec3 p0 = {0,0,-3};
        vec3 p0 = {0,0,scene.camera.scale-0.1f};
        vec3 p1 = {0,0,-scene.camera.scale - 2};
        //const picking_info info0 = ray_intersect_plane(r,{0,0,1}, p0);
        //const picking_info info1 = ray_intersect_plane(r,{0,0,1}, p1);
        const picking_info info0 = ray_intersect_plane(r,n, scene.camera.orientation*p0);
        const picking_info info1 = ray_intersect_plane(r,n, scene.camera.orientation*p1);


        p0 = info0.intersection;
        p1 = info1.intersection;

        float eps = 0.5f;


        //Dichotomia to find the intersection of the ray and the ground

        if ((p0.z-height(p0.x, p0.y)[2])*(p1.z-height(p1.x, p1.y)[2]) <= 0)
        {
            //int N = (int) std::log2(std::sqrt(dot(p0-p1, p0-p1)) / eps);
            //while (std::sqrt(dot(p0,p1)) > eps)
            //for(int i = 0; i<N; i++)
            while (std::sqrt(dot(p0-p1, p0-p1)) > eps)
            {
                const vec3& p2 = (p0+p1)/2;
                if ((p0.z-height(p0.x, p0.y)[2])*(p2.z-height(p2.x, p2.y)[2]) <= 0)
                    p1 = p2;
                else
                    p0 = p2;
            }

            vegetation_position[gui_ter.veg_ind].push_back(height(p0.x, p0.y) - vec3(0,0,0.2f));
            if(gui_ter.veg_ind == 0)
            {
                palmtrees_orientation.push_back(rand_interval(0.0f, 2*M_PI));
                vegetation_scaling[0].push_back(rand_interval(0.7f,1.8f));
            }
            else if(gui_ter.veg_ind == 1)
                vegetation_scaling[1].push_back(rand_interval(1.0f,2.0f));
            else if(gui_ter.veg_ind == 2)
                vegetation_scaling[2].push_back(rand_interval(0.5f,1.0f));
            else if(gui_ter.veg_ind == 3)
            {
                bushes_orientation.push_back(rand_interval(0.0f, 2*M_PI));
                vegetation_scaling[3].push_back(rand_interval(0.5f,1.2f));
            }
        }
    }
}
