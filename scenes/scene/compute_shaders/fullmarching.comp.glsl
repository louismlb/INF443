
#extension GL_ARB_shader_atomic_counter_ops : enable

// #include "scenes/3D_graphics/08_marching_cubes/compute_shaders/marchtable.glsl"

// int numThreads = 8;

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

// mef, should only send a buffer with the grid data, accessed with arrayID,
// instead of vectors with xyz easy to deduce...
layout (binding=1) buffer Grid {float GridData[];};

layout(binding = 7) uniform atomic_uint triCount;

layout(binding = 9) buffer T
{
        int triangulation[];
};

uniform vec3 corner;
uniform float stride;

uniform int numPointsPerAxis;
uniform float isoLevel;

struct Triangle {
    vec4 a;
    vec4 b;
    vec4 c;
};

layout(std430, binding = 8) buffer t {Triangle triangles[];};




uint arrayID(uvec3 p) {
    return p.z * numPointsPerAxis * numPointsPerAxis + p.y * numPointsPerAxis + p.x;
}


vec3 interpolate(float level, vec3 p1, vec3 p2, float v1, float v2) {
    if (abs(level-v1) < 0.00001)
    {
        return(p1);

    }
    if (abs(level-v2) < 0.00001)
    {
        return(p2);

    }
    if (abs(v1-v2) < 0.00001)
    {
        return(p1);

    }
    float t = (level - v1) / (v2 - v1);
    return p1 + t * (p2-p1);
}


void main () // used to be called March
{
    uvec3 gid = gl_GlobalInvocationID;

    // Stop one point before the end because voxel includes neighbouring points
    if (gid.x >= numPointsPerAxis-1 || gid.y >= numPointsPerAxis-1 || gid.z >= numPointsPerAxis-1) {
        return;
    }

    uvec3 gridPos[8];

    gridPos[0] = gid;
    gridPos[1] = gid + uvec3(1,0,0);
    gridPos[2] = gid + uvec3(1,1,0);
    gridPos[3] = gid + uvec3(0,1,0);
    gridPos[4] = gid + uvec3(0,0,1);
    gridPos[5] = gid + uvec3(1,0,1);
    gridPos[6] = gid + uvec3(1,1,1);
    gridPos[7] = gid + uvec3(0,1,1);

    // 8 corners of the current cube
    float cubeCorners[8];
    for (uint i = 0; i < 8; ++i)
    {
        cubeCorners[i] = GridData[arrayID(gridPos[i])];
    }

    // Calculate unique index for each cube configuration.
    // There are 256 possible values
    // A value of 0 means cube is entirely inside surface; 255 entirely outside.
    // The value is used to look up the edge table, which indicates which edges of the cube are cut by the isosurface.
    int cubeIndex = 0;
    if (cubeCorners[0] < isoLevel) cubeIndex |= 1;
    if (cubeCorners[1] < isoLevel) cubeIndex |= 2;
    if (cubeCorners[2] < isoLevel) cubeIndex |= 4;
    if (cubeCorners[3] < isoLevel) cubeIndex |= 8;
    if (cubeCorners[4] < isoLevel) cubeIndex |= 16;
    if (cubeCorners[5] < isoLevel) cubeIndex |= 32;
    if (cubeCorners[6] < isoLevel) cubeIndex |= 64;
    if (cubeCorners[7] < isoLevel) cubeIndex |= 128;

    if (cubeIndex != 0 && cubeIndex != 255)
    {
        //atomicCounterAddARB(pointCounter, totalTable[cubeIndex]);
        //uint bufferIndex = atomicCounterIncrement(marchableCounter);
        //uvec4 mc = uvec4(gid.x,gid.y,gid.z,cubeIndex);
        //marchableList[bufferIndex] = mc;

        // Create triangles for current cube configuration
        for (int i = 0; triangulation[cubeIndex][i] != -1; i +=3) {
            uint bufferIndex = atomicCounterIncrement(triCount);
            // Get indices of corner points A and B for each of the three edges
            // of the cube that need to be joined to form the triangle.
            int a0 = cornerIndexAFromEdge[triangulation[cubeIndex*16+i]];
            int b0 = cornerIndexBFromEdge[triangulation[cubeIndex*16+i]];

            int a1 = cornerIndexAFromEdge[triangulation[cubeIndex*16+i+1]];
            int b1 = cornerIndexBFromEdge[triangulation[cubeIndex*16+i+1]];

            int a2 = cornerIndexAFromEdge[triangulation[cubeIndex*16+i+2]];
            int b2 = cornerIndexBFromEdge[triangulation[cubeIndex*16+i+2]];

            Triangle tri;
            tri.a = vec4(interpolate(isoLevel, gridPos[a0], gridPos[b0], cubeCorners[a0], cubeCorners[b0])*stride+corner, 0);
            tri.b = vec4(interpolate(isoLevel, gridPos[a1], gridPos[b1], cubeCorners[a1], cubeCorners[b1])*stride+corner, 0);
            tri.c = vec4(interpolate(isoLevel, gridPos[a2], gridPos[b2], cubeCorners[a2], cubeCorners[b2])*stride+corner, 0);
            triangles[bufferIndex] = tri;
        }
    }
}

