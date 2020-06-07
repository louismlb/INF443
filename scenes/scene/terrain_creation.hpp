#pragma once


#include "main/scene_base/base.hpp"
#include "marching_cubes.hpp"




struct int2 {
    int x;
    int y;
};



struct mesh_generator
{
    //Heightmap generator settings
    int heightmap_seed = 10;              //No default.
    bool randomizeSeed = false; //No default

    int numOctaves = 7;
    float persistence = .5f;
    float lacunarity = 2;
    float initialScale = 2;

    GLuint offsets_ssbo;
    GLuint minmax_ssbo;
    GLuint map_ssbo;

    vcl::buffer<float> Generate_Heightmap_CPU(int mapSize);

    vcl::buffer<float> Generate_Heightmap_GPU(int mapSizeValue);

    GLuint heightmap_program = 0;

    //Other settings

    //[Header ("Mesh Settings")]
    //[Range (2, 255)]
    int mapSize = 512; // 255
    float scale = 20;
    float elevationScale = 4; // 10
    //Material material;

    //[Header ("Erosion Settings")]
    int numErosionIterations = 1*1024; // 50000

    //[Header ("Animation Settings")]
    bool animateErosion;
    int iterationsPerFrame = 100; // 100
    bool showNumIterations;
    int numAnimatedErosionIterations;

    // New vars
    bool printTimers;
    //ComputeShader erosion;
    int erosionBrushRadius = 3;
    int maxLifetime = 30;
    float sedimentCapacityFactor = 3;
    float minSedimentCapacity = .01f;
    float depositSpeed = 0.3f;
    float erodeSpeed = 0.3f;
    float evaporateSpeed = .01f;
    float gravity = 4;
    float startSpeed = 1;
    float startWater = 1;
    float inertia = 0.3f;

    int mapSizeWithBorder;

    int erosion_seed = 10;
    // End New vars

    vcl::buffer<float> map; // or std::vector?

    vcl::mesh mesh;

    //heightmap_generator heightmap;
    //erosion_generator erosion;

    GLuint erosion_program = 0;

    GLuint brushIndexBuffer;
    GLuint brushWeightBuffer;
    GLuint randomIndexBuffer;
    GLuint mapBuffer;

    vcl::vec3 CalculateHeightAndGradient (vcl::buffer<float>& nodes, int mapSize, float posX, float posY);


    //MeshRenderer meshRenderer;
    //MeshFilter meshFilter;

    //void Start();

    //void StartMeshGeneration();

    //void Erode();

    void Start_Map();

    void Heightmap_GPU();

    void Heightmap_CPU();

    void Erode_GPU();

    void Erode_CPU();

    //void UpdateErosion();

    void GenerateMesh();

    void UpdateMesh();

};

struct noise_generator
{


};



