
#extension GL_ARB_shader_atomic_counter_ops : enable

// #include "scenes/3D_graphics/08_marching_cubes/compute_shaders/marchtable.glsl"

// int numThreads = 8;

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

// mef, should only send a buffer with the grid data, accessed with arrayID,
// instead of vectors with xyz easy to deduce...
layout (binding=1) buffer Grid {float points[];};

layout (binding=2) uniform atomic_uint marchableCounter;

layout (binding=3) uniform atomic_uint pointCounter;

layout (std430, binding = 4) buffer Y {uvec4 marchableList[];};


uniform int numPointsPerAxis;
uniform float isoLevel;


uint arrayID(uvec3 p) {
    return p.z * numPointsPerAxis * numPointsPerAxis + p.y * numPointsPerAxis + p.x;
}



void main () // used to be called March
{
    uvec3 gid = gl_GlobalInvocationID;

    // Stop one point before the end because voxel includes neighbouring points
    if (gid.x >= numPointsPerAxis-1 || gid.y >= numPointsPerAxis-1 || gid.z >= numPointsPerAxis-1) {
        return;
    }

    // 8 corners of the current cube
    float cubeCorners[8] = {
        points[arrayID(gid)],
        points[arrayID(gid + uvec3(1,0,0))],
        points[arrayID(gid + uvec3(1,1,0))],
        points[arrayID(gid + uvec3(0,1,0))],
        points[arrayID(gid + uvec3(0,0,1))],
        points[arrayID(gid + uvec3(1,0,1))],
        points[arrayID(gid + uvec3(1,1,1))],
        points[arrayID(gid + uvec3(0,1,1))]
    };

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
        atomicCounterAddARB(pointCounter, totalTable[cubeIndex]);
        uint bufferIndex = atomicCounterIncrement(marchableCounter);
        uvec4 mc = uvec4(gid.x,gid.y,gid.z,cubeIndex);
        marchableList[bufferIndex] = mc;
    }

}

