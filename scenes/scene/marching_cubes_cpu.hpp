#pragma once

#include "main/scene_base/base.hpp"




class NoiseDensityCPU
{
public:
    // Density Generator stuff

    int seed = 1; // No default
    int numOctaves = 8;         // 4
    float lacunarity = 2;       // 2
    float persistence = .5f;    // .5f
    float noiseScale = 2;       // 1
    float noiseWeight = 5;      // 1
    float floorOffset = 0;      // 1   4 is good
    float weightMultiplier = 5; // 1

    int closeEdges = 1; // no default, uses ints to store a bool in openGL
    float hardFloorHeight = 0; // no default
    float hardFloorWeight = 2; // no default

    vcl::vec4 shaderParams = vcl::vec4(1,1,1,1); // no default

    // Override no more
    void Generate (vcl::buffer<float>& pointsBuffer, uint numPointsPerAxis, float boundsSize,
                                 vcl::vec3 worldBounds, vcl::vec3 corner, vcl::vec3 offset, float stride);

protected:
    // Not useful?
    //vcl::buffer<vcl::ComputeBuffer> buffersToRelease;
};


class MeshGeneratorCPU
{

public:

    vcl::vec3 worldSize = {5,4,3}; // no of chunks
    vcl::vec3 noiseCorner = {0,0,0}; // to evaluate the noise
    vcl::vec3 cornerPoint = {-20,-20,-5}; // to place the vertices
    // stop looking at chunks from their center point.
    // Could also have center point and do center - boundsSize/2 to find corner
    vcl::vec3 Pointoffset = {0,0,0};

    NoiseDensityCPU densityGenerator;

    vcl::mesh generator_mesh;

    float isoLevel = 0.0f; // No default.. 0.5f
    float boundsSize = 40; // physical size of chunk

    vcl::vec3 worldBounds = worldSize * boundsSize;

    //[Range (2, 100)]
    uint numPointsPerAxis = 160;
    uint numVoxelsPerAxis = numPointsPerAxis - 1;
    uint numPoints = numPointsPerAxis * numPointsPerAxis * numPointsPerAxis;
    uint numVoxels = numVoxelsPerAxis * numVoxelsPerAxis * numVoxelsPerAxis;

    float stride = boundsSize / numVoxelsPerAxis; // size of voxel

    //[Header ("Gizmos")]
    //bool showBoundsGizmo = true;
    //Color boundsGizmoCol = Color.white;

    void GenerateBuffer();

    void GenerateMesh();

private:

    // Buffers
    vcl::buffer<float> points;

    uint triCount;

    vcl::buffer<std::array<uint, 3>> triangles;

    vcl::buffer<vcl::vec3> vertices;

    //vcl::buffer<vcl::vec4> colors;

    vcl::buffer<vcl::vec3> normals;

};


