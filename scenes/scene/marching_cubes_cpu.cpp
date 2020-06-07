
#include "marching_cubes_cpu.hpp"
#include "tables.hpp"

using namespace vcl;

uint arrayID(uint x, uint y, uint z, uint numPointsPerAxis) {
    return z * numPointsPerAxis * numPointsPerAxis + y * numPointsPerAxis + x;
}

uint arrayID(uvec3 p, uint numPointsPerAxis) {
    return p.z * numPointsPerAxis * numPointsPerAxis + p.y * numPointsPerAxis + p.x;
}

vec3 interpolate(float level, vec3 p1, vec3 p2, float v1, float v2) {
    if (std::abs(level-v1) < 0.00001)
    {
        return(p1);

    }
    if (std::abs(level-v2) < 0.00001)
    {
        return(p2);

    }
    if (std::abs(v1-v2) < 0.00001)
    {
        return(p1);

    }
    float t = (level - v1) / (v2 - v1);
    return p1 + t * (p2-p1);
}

/*
vec4 colorFromHeight(float z) {
    vec4 newcolor;
    if (z > 10) {
        newcolor = vec4(252, 252, 245, 255);
    }
    else if (z > 8) {
        newcolor = vec4(97, 71, 0, 255);
    }
    else if (z > 4) {
        newcolor = vec4(145, 127, 77, 255);
    }
    else if (z > 2) {
        newcolor = vec4(24, 87, 22, 255);
    }
    else {
        newcolor = vec4(252, 186, 3, 255);
    }
    return newcolor;
}
*/


void NoiseDensityCPU::Generate (buffer<float>& pointsBuffer, uint numPointsPerAxis, float boundsSize, vec3 worldBounds, vec3 corner, vec3 offset, float stride) {
    //buffersToRelease = new List<ComputeBuffer> ();

    // Noise parameters
    int gen_seed = 0;
    std::mt19937                        generator(gen_seed);
    std::uniform_real_distribution<>    gen_distr(-1,1);

    buffer<vec3> offsets; offsets.resize(numOctaves);

    float offsetRange = 1000;
    for (int i = 0; i < numOctaves; i++) {
        offsets[i] = vec3(gen_distr(generator), gen_distr(generator), gen_distr(generator)) * offsetRange;
    }

    // Dispatch shader
    for (uint x=0; x<numPointsPerAxis; ++x)
    {
        for (uint y=0; y<numPointsPerAxis; ++y)
        {
            for (uint z=0; z<numPointsPerAxis; ++z)
            {
                vec3 id = vec3(x,y,z);
                uint index = arrayID(x,y,z, numPointsPerAxis);

                vec3 pos = corner + id * stride; // corner = center - boundsSize/2, so boundsSize is unused now
                float offsetNoise = 0;

                float noise = 0;

                float frequency = noiseScale/100;
                float amplitude = 1;
                float weight = 1;
                for (int j =0; j < numOctaves; j ++) {
                    vec3 arg = (pos+offsetNoise) * frequency + offsets[j] + offset;
                    float n = snoise3(arg.x, arg.y, arg.z);
                    float v = 1 - std::abs(n);
                    v = v*v;
                    v *= weight;
                    weight = std::max<float>(std::min<float>(v*weightMultiplier,1),0); // or clamp between 0 and 1
                    noise += v * amplitude;
                    amplitude *= persistence;
                    frequency *= lacunarity;
                }

                // terraces
                //float finalVal = -(pos.z + floorOffset) + noise * noiseWeight + fmod(pos.z, shaderParams.x) * shaderParams.y; // (pos.z%params.x) operands to '%' must be integral

                float finalVal = -(pos.z + floorOffset) + noise * noiseWeight;

                if (pos.z < hardFloorHeight) {
                    finalVal += hardFloorWeight;
                }

                if (closeEdges == 1) {
                    vec3 edgeOffset = vec3(std::abs(pos.x*2) - worldBounds.x + stride/2, std::abs(pos.y*2) - worldBounds.y + stride/2, std::abs(pos.z*2) - worldBounds.z + stride/2);
                    float edgeWeight = (std::max<float>( std::max<float>( edgeOffset.x,edgeOffset.y ), edgeOffset.z ) < 0.0) ? 0.0 : 1.0 ;
                    finalVal = finalVal * (1-edgeWeight) - 100 * edgeWeight;

                }

                pointsBuffer[index] = finalVal; // vec4(pos, finalVal)

            }
        }
    }
}



void MeshGeneratorCPU::GenerateBuffer () {

    points.resize(numPoints);

    densityGenerator.Generate(points, numPointsPerAxis, boundsSize, worldBounds, noiseCorner, Pointoffset, stride); opengl_debug();
}



void MeshGeneratorCPU::GenerateMesh () {

    triCount = 0;

    for (uint x=0; x<numVoxelsPerAxis; ++x)
    {
        for (uint y=0; y<numVoxelsPerAxis; ++y)
        {
            for (uint z=0; z<numVoxelsPerAxis; ++z)
            {
                uvec3 gid = uvec3(x,y,z);

                uvec3 gridPos[8];
                gridPos[0] = gid;
                gridPos[1] = gid + uvec3(1,0,0);
                gridPos[2] = gid + uvec3(1,1,0);
                gridPos[3] = gid + uvec3(0,1,0);
                gridPos[4] = gid + uvec3(0,0,1);
                gridPos[5] = gid + uvec3(1,0,1);
                gridPos[6] = gid + uvec3(1,1,1);
                gridPos[7] = gid + uvec3(0,1,1);

                vec3 gridPosF[8];
                gridPosF[0] = vec3(x,y,z);
                gridPosF[1] = vec3(x+1,y+0,z+0);
                gridPosF[2] = vec3(x+1,y+1,z+0);
                gridPosF[3] = vec3(x+0,y+1,z+0);
                gridPosF[4] = vec3(x+0,y+0,z+1);
                gridPosF[5] = vec3(x+1,y+0,z+1);
                gridPosF[6] = vec3(x+1,y+1,z+1);
                gridPosF[7] = vec3(x+0,y+1,z+1);

                float cubeCorners[8];
                for (uint i = 0; i < 8; ++i)
                {
                    cubeCorners[i] = points[arrayID(gridPos[i], numPointsPerAxis)];
                }

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
                    // Create triangles for current cube configuration
                    for (int i = 0; Tables::triangulationTable[cubeIndex][i] != -1; i +=3) {
                        // Get indices of corner points A and B for each of the three edges
                        // of the cube that need to be joined to form the triangle.
                        int a0 = Tables::cornerIndexAFromEdge[Tables::triangulationTable[cubeIndex][i]];
                        int b0 = Tables::cornerIndexBFromEdge[Tables::triangulationTable[cubeIndex][i]];

                        int a1 = Tables::cornerIndexAFromEdge[Tables::triangulationTable[cubeIndex][i+1]];
                        int b1 = Tables::cornerIndexBFromEdge[Tables::triangulationTable[cubeIndex][i+1]];

                        int a2 = Tables::cornerIndexAFromEdge[Tables::triangulationTable[cubeIndex][i+2]];
                        int b2 = Tables::cornerIndexBFromEdge[Tables::triangulationTable[cubeIndex][i+2]];

                        vec3 p = interpolate(isoLevel, gridPosF[a0], gridPosF[b0], cubeCorners[a0], cubeCorners[b0]) * stride + cornerPoint;
                        //vec4 color = colorFromHeight(p.z);
                        vertices.push_back(p);
                        //colors.push_back(color);

                        p = interpolate(isoLevel, gridPosF[a1], gridPosF[b1], cubeCorners[a1], cubeCorners[b1]) * stride + cornerPoint;
                        //color = colorFromHeight(p.z);
                        vertices.push_back(p);
                        //colors.push_back(color);

                        p = interpolate(isoLevel, gridPosF[a2], gridPosF[b2], cubeCorners[a2], cubeCorners[b2]) * stride + cornerPoint;
                        //color = colorFromHeight(p.z);
                        vertices.push_back(p);
                        //colors.push_back(color);

                        //vertices.push_back( interpolate(isoLevel, gridPosF[a0], gridPosF[b0], cubeCorners[a0], cubeCorners[b0]) * stride + cornerPoint ) ;
                        //vertices.push_back( interpolate(isoLevel, gridPosF[a1], gridPosF[b1], cubeCorners[a1], cubeCorners[b1]) * stride + cornerPoint ) ;
                        //vertices.push_back( interpolate(isoLevel, gridPosF[a2], gridPosF[b2], cubeCorners[a2], cubeCorners[b2]) * stride + cornerPoint ) ;

                        triangles.push_back({triCount, triCount + 1, triCount + 2});
                        triCount += 3;
                    }
                }

            }
        }
    }

    generator_mesh.position = vertices;
    //generator_mesh.color = colors;
    generator_mesh.connectivity = triangles;

}

