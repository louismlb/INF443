#pragma once

#include "main/scene_base/base.hpp"


struct Triangle {
//#pragma warning disable 649 // disable unassigned variable warning
    vcl::vec4 a;
    vcl::vec4 b;
    vcl::vec4 c;

    // implement a method to get a,b or c when calling 0, 1, 2
    const vcl::vec4& operator[](std::size_t index) const;
    vcl::vec4& operator[](std::size_t index);
};


class NoiseDensityBis
{
public:
    // Density Generator stuff
    const int threadGroupSize = 8;
    vcl::ComputeShaderProgram densityShader;


    int seed = 1; // No default
    int numOctaves = 4;
    float lacunarity = 2;
    float persistence = .5f;
    float noiseScale = 1;
    float noiseWeight = 1;
    float floorOffset = 1;
    float weightMultiplier = 1;

    int closeEdges = 1; // no default, uses ints to store a bool in openGL
    float hardFloorHeight = 4; // no default
    float hardFloorWeight = 3; // no default

    vcl::vec4 shaderParams = vcl::vec4(1,1,1,1); // no default

    // Override no more
    vcl::ComputeBuffer Generate (vcl::ComputeBuffer& pointsBuffer, int numPointsPerAxis, float boundsSize,
                                 vcl::vec3 worldBounds, vcl::vec3 corner, vcl::vec3 offset, float spacing);

protected:
    // Not useful?
    //vcl::buffer<vcl::ComputeBuffer> buffersToRelease;
};


class MeshGeneratorBis
{

    const int threadGroupSize = 8;

public:

    vcl::vec3 worldSize = {5,4,3}; // no of chunks
    vcl::vec3 cornerPoint = {0,0,0}; // stop looking at chunks from their center point.
    // Could also have center point and do center - boundsSize/2 to find corner
    vcl::vec3 Pointoffset = {0,0,0};

    NoiseDensityBis densityGenerator;

    vcl::ComputeShaderProgram marching_shader;
    vcl::ComputeShaderProgram triangulation_shader;

    vcl::mesh generator_mesh;

    float isoLevel = 0.5f; // No default..
    float boundsSize = 1; // physical size of chunk

    vcl::vec3 worldBounds = worldSize * boundsSize;

    //[Range (2, 100)]
    int numPointsPerAxis = 30;

    int numVoxelsPerAxis = numPointsPerAxis - 1;

    float stride = boundsSize / numVoxelsPerAxis; // size of voxel

    //[Header ("Gizmos")]
    //bool showBoundsGizmo = true;
    //Color boundsGizmoCol = Color.white;

    void GenerateBuffer();

    void GenerateMesh();

private:

    // Buffers
    vcl::ComputeBuffer triTableBuffer;

    vcl::ComputeBuffer pointCounter;
    vcl::ComputeBuffer pointSSBO;

    vcl::ComputeBuffer marchableCounter;
    vcl::ComputeBuffer marchableSSBO;

    vcl::ComputeBuffer triangleCounter;

    vcl::ComputeBuffer triangleSSBO;
    vcl::buffer<Triangle> triangles;

    vcl::ComputeBuffer vertexSSBO;
    vcl::buffer<vcl::vec4> vertices;

    vcl::ComputeBuffer normalSSBO;
    vcl::buffer<vcl::vec4> normals;

    //vcl::ComputeBuffer triCountBuffer;

    void CreateBuffers ();

    //void ReleaseBuffers ();

};


