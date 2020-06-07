
// #include "scenes/3D_graphics/08_marching_cubes/compute_shaders/marchtable.glsl"

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

/*
struct Triangle {
    vec3 vertexC;
    vec3 vertexB;
    vec3 vertexA;
};

layout(std430, binding=2) buffer trianglesBuffer {
    Triangle[] triangles;
};
*/

layout(binding=1) buffer Grid
{
        float GridData[];
};
uniform uint marchableCount;
uniform uint pointCount;
uniform vec3 corner;
uniform float stride;

layout(std430, binding = 4) buffer X {uvec4 marchableList[];}; //format is x,y,z,cubeIndex

layout(std430, binding = 5) buffer v {vec4 vertices[];};
layout(std430, binding = 6) buffer n {vec4 normals[];};
layout(std430, binding = 8) buffer t {uvec4 triangles[];};

layout(binding = 7) uniform atomic_uint triCount;

layout(binding = 9) buffer T
{
        int triangulation[];
};


uniform int numPointsPerAxis;


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


uint arrayID(uvec3 p) {
    return p.z * numPointsPerAxis * numPointsPerAxis + p.y * numPointsPerAxis + p.x;
}



void main () // used to be called March
{
    uvec3 id = gl_GlobalInvocationID;

    if (id.x < marchableCount)
    {
        uvec4 marchableItem = marchableList[id.x];
        uvec3 gid = marchableItem.xyz;
        uint cubeIndex = marchableItem.w;

        vec3 vertlist[12];
        for (int filler = 0; filler < 12; filler++) // used to be commented...
        {
            vertlist[filler]=vec3(0.);
        }

        float gridCells[8];
        uvec3 gridPos[8];

        gridPos[0] = gid;
        gridPos[1] = gid + uvec3(1,0,0);
        gridPos[2] = gid + uvec3(1,1,0);
        gridPos[3] = gid + uvec3(0,1,0);
        gridPos[4] = gid + uvec3(0,0,1);
        gridPos[5] = gid + uvec3(1,0,1);
        gridPos[6] = gid + uvec3(1,1,1);
        gridPos[7] = gid + uvec3(0,1,1);

        for (int i = 0; i < 8; i++)
        {
                gridCells[i] = GridData[arrayID(gridPos[i])];
        }

        int E = int(edges[cubeIndex]);
        if (E != 0 && E != 255)
        {
            // cubeIndex was created inside if before
            /* Find the vertices where the surface intersects the cube */
            if ((E & 1) != 0)
            {
                vertlist[0] = interpolate(0,gridPos[0],gridPos[1],gridCells[0],gridCells[1]);
            }
            if ((E & 2) != 0)
            {
                vertlist[1] = interpolate(0,gridPos[1],gridPos[2],gridCells[1],gridCells[2]);
            }
            if ((E & 4) != 0)
            {
                vertlist[2] = interpolate(0,gridPos[2],gridPos[3],gridCells[2],gridCells[3]);
            }
            if ((E & 8) != 0)
            {
                vertlist[3] = interpolate(0,gridPos[3],gridPos[0],gridCells[3],gridCells[0]);
            }
            if ((E & 16) != 0)
            {
                vertlist[4] = interpolate(0,gridPos[4],gridPos[5],gridCells[4],gridCells[5]);
            }
            if ((E & 32) != 0)
            {
                vertlist[5] = interpolate(0,gridPos[5],gridPos[6],gridCells[5],gridCells[6]);
            }
            if ((E & 64) != 0)
            {
                vertlist[6] = interpolate(0,gridPos[6],gridPos[7],gridCells[6],gridCells[7]);
            }
            if ((E & 128) != 0)
            {
                vertlist[7] = interpolate(0,gridPos[7],gridPos[4],gridCells[7],gridCells[4]);
            }
            if ((E & 256) != 0)
            {
                vertlist[8] = interpolate(0,gridPos[0],gridPos[4],gridCells[0],gridCells[4]);
            }
            if ((E & 512) != 0)
            {
                vertlist[9] = interpolate(0,gridPos[1],gridPos[5],gridCells[1],gridCells[5]);
            }
            if ((E & 1024) != 0)
            {
                vertlist[10] = interpolate(0,gridPos[2],gridPos[6],gridCells[2],gridCells[6]);
            }
            if ((E & 2048) != 0)
            {
                vertlist[11] = interpolate(0,gridPos[3],gridPos[7],gridCells[3],gridCells[7]);
            }

            for (int tCount = 0; triangulation[cubeIndex*16+tCount] != -1; tCount+=3)
            {
                uint index = atomicCounterIncrement(triCount);

                for (int t = 0; t < 3;t++)
                {
                    vec3 vertPos = vertlist[triangulation[cubeIndex*16+tCount+t]]*stride+corner;
                    vertices[3*index+t] = vec4(vertPos,1);//vertPos;

                    //float eps = 0.1;
                    //float dX = (distFunc(vertPos+vec3(eps,0,0))-distFunc(vertPos));
                    //float dY = (distFunc(vertPos+vec3(0,eps,0))-distFunc(vertPos));
                    //float dZ = (distFunc(vertPos+vec3(0,0,eps))-distFunc(vertPos));

                    //normals[3*index+t] = normalize(vec4(dX,dY,dZ,0));
                }

                //triangles[index] = uvec4(3*index, 3*index+1, 3*index+2, 0);
            }
        }
    }
}

// Have to transform the 3D id pos with this!
// vec3 pos = centre + id * spacing - boundsSize/2;


    /* In main, after cubeIndex is found
      Beware : old interp function,
    // Create triangles for current cube configuration
    for (int i = 0; triangulation[cubeIndex][i] != -1; i +=3) {
        // Get indices of corner points A and B for each of the three edges
        // of the cube that need to be joined to form the triangle.
        int a0 = cornerIndexAFromEdge[triangulation[cubeIndex][i]];
        int b0 = cornerIndexBFromEdge[triangulation[cubeIndex][i]];

        int a1 = cornerIndexAFromEdge[triangulation[cubeIndex][i+1]];
        int b1 = cornerIndexBFromEdge[triangulation[cubeIndex][i+1]];

        int a2 = cornerIndexAFromEdge[triangulation[cubeIndex][i+2]];
        int b2 = cornerIndexBFromEdge[triangulation[cubeIndex][i+2]];

        Triangle tri;
        tri.vertexA = interpolateVerts(cubeCorners[a0], cubeCorners[b0]);
        tri.vertexB = interpolateVerts(cubeCorners[a1], cubeCorners[b1]);
        tri.vertexC = interpolateVerts(cubeCorners[a2], cubeCorners[b2]);
        triangles.Append(tri);
    } */

