
// #include "scenes/3D_graphics/08_marching_cubes/compute_shaders/noise.glsl"

// int numThreads = 8; error if I pass this to local_size

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;


// Noise settings
layout(std430, binding=0) buffer offsetsBuffer {
    vec4[] offsets;
};

uniform vec4 params;

uniform int octaves;
uniform int closeEdges;

uniform float lacunarity;
uniform float persistence;
uniform float noiseScale;
uniform float noiseWeight;
uniform float floorOffset;
uniform float weightMultiplier;
uniform float hardFloor;
uniform float hardFloorWeight;

layout(std430, binding=1) buffer pointsBuffer {
    float[] points;
};

uniform vec3 corner;
uniform vec3 offset;
uniform vec3 worldSize;

uniform int numPointsPerAxis;
uniform float boundsSize;
uniform float stride;


uint indexFromCoord(uint x, uint y, uint z) {
    return z * numPointsPerAxis * numPointsPerAxis + y * numPointsPerAxis + x;
}

void main () // used to be called Density
{
    uvec3 id = gl_GlobalInvocationID;

    if (id.x >= numPointsPerAxis || id.y >= numPointsPerAxis || id.z >= numPointsPerAxis) {
        return;
    }

    //    uint index = indexFromCoord(id.x,id.y,id.z);
    //    points[index] = 1.0;
    //    return;               Test, fails..

    vec3 pos = corner + id * stride; // corner = center - boundsSize/2
    float offsetNoise = 0;

    float noise = 0;

    float frequency = noiseScale/100;
    float amplitude = 1;
    float weight = 1;
    for (int j =0; j < octaves; j++) {
        float n = snoise((pos+offsetNoise) * frequency + offsets[j].xyz + offset);
        float v = 1-abs(n);
        v = v*v;
        v *= weight;
        weight = max(min(v*weightMultiplier,1),0); // or clamp between 0 and 1
        noise += v * amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    float finalVal = -(pos.z + floorOffset) + noise * noiseWeight + mod(pos.z,params.x) * params.y; // (pos.z%params.x) operands to '%' must be integral


    if (pos.y < hardFloor) {
        finalVal += hardFloorWeight;
    }

    if (closeEdges == 1) {
        vec3 edgeOffset = abs(pos*2) - worldSize + stride/2;
        float edgeWeight = clamp( sign( max( max( edgeOffset.x,edgeOffset.y ), edgeOffset.z ) ), 0.0, 1.0 );
        finalVal = finalVal * (1-edgeWeight) - 100 * edgeWeight;

    }

/*
    uint index = indexFromCoord(id.x,id.y,id.z);
    points[index] = finalVal; // vec4(pos, finalVal)
*/

}

