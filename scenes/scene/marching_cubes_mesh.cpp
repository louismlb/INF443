
#include "marching_cubes_mesh.hpp"
#include "tables.hpp"


using namespace vcl;


// Used to return c on default case instead of case 2
const vec4& Triangle::operator[](std::size_t index) const
{
    switch(index) {
    case 0:
        return a;
    case 1:
        return b;
    case 2:
        return c;
    default:
        error_vcl("Try to access Triangle["+std::to_string(index)+"]");
    }

}

vec4& Triangle::operator[](std::size_t index)
{
    switch(index) {
    case 0:
        return a;
    case 1:
        return b;
    case 2:
        return c;
    default:
        error_vcl("Try to access Triangle["+std::to_string(index)+"]");
    }

}


void MeshGeneratorBis::CreateBuffers () {
    int numPoints = numPointsPerAxis * numPointsPerAxis * numPointsPerAxis;
    int numVoxels = numVoxelsPerAxis * numVoxelsPerAxis * numVoxelsPerAxis;

    // One pass with a maxTriangleCount buffer
    // Or 2 passes, first one gives no of triangles and before second one
    // You set size of buffer
    int maxTriangleCount = numVoxels * 5;
    // max 5 triangles per Voxel with marching cubes?

    pointSSBO.Generate(); // Without this the SetData gives GL_INVALID_OPERATION because buffer is bound to 0
    pointSSBO.SetParams(numPoints, sizeof(float));
    pointSSBO.SetTarget(GL_SHADER_STORAGE_BUFFER);

    triangleSSBO.Generate();
    triangleSSBO.SetParams(maxTriangleCount, sizeof(Triangle));
    // Could be bad to set size of buffer in the loop??
    // Or just generate the chunk when camera arrives nearby, doesn't take too long after
    triangleSSBO.SetTarget(GL_SHADER_STORAGE_BUFFER);

    normalSSBO.Generate();
    normalSSBO.SetTarget(GL_SHADER_STORAGE_BUFFER);

    triangleCounter.Generate();
    triangleCounter.SetParams(1, sizeof(GLuint));
    triangleCounter.SetTarget(GL_ATOMIC_COUNTER_BUFFER);

    triTableBuffer.Generate();
    triTableBuffer.SetParams(4096, sizeof(int));
    triTableBuffer.SetTarget(GL_SHADER_STORAGE_BUFFER);
    triTableBuffer.BindBase(8);
    triTableBuffer.SetData(Tables::triTable, GL_DYNAMIC_DRAW);
}



ComputeBuffer NoiseDensityBis::Generate (ComputeBuffer& pointsBuffer, int numPointsPerAxis, float boundsSize, vec3 worldBounds, vec3 corner, vec3 offset, float spacing) {
    //buffersToRelease = new List<ComputeBuffer> ();

    // Noise parameters
    int gen_seed = 0;
    std::mt19937                        generator(gen_seed);
    std::uniform_real_distribution<>    gen_distr(-1,1);

    buffer<vec4> offsets; offsets.resize(numOctaves);

    float offsetRange = 1000;
    for (int i = 0; i < numOctaves; i++) {
        offsets[i] = vec4(gen_distr(generator), gen_distr(generator), gen_distr(generator), 0) * offsetRange;
    }

    ComputeBuffer offsetsBuffer;
    offsetsBuffer.Generate();
    offsetsBuffer.SetParams(offsets.size(), sizeof (float) * 4);
    offsetsBuffer.SetTarget(GL_SHADER_STORAGE_BUFFER);
    offsetsBuffer.SetData(&offsets[0], GL_DYNAMIC_DRAW);

    //buffersToRelease.push_back(offsetsBuffer);

    //densityShader.SetBuffer(0, "offsets", offsetsBuffer);
    offsetsBuffer.BindBase(0);

    densityShader.Run();

    densityShader.UniformVecF("params", 1, 4, &shaderParams[0]);
    //densityShader.UniformVecF("centre", 1, 4, &vec4(centre.x, centre.y, centre.z, 1)[0]);

    densityShader.UniformI("octaves", std::max(1, numOctaves));
    densityShader.UniformI("closeEdges", closeEdges); //bool...

    densityShader.UniformF("lacunarity", lacunarity);
    densityShader.UniformF("persistence", persistence);
    densityShader.UniformF("noiseScale", noiseScale);
    densityShader.UniformF("noiseWeight", noiseWeight);
    densityShader.UniformF("floorOffset", floorOffset);
    densityShader.UniformF("weightMultiplier", weightMultiplier);
    densityShader.UniformF("hardFloor", hardFloorHeight);
    densityShader.UniformF("hardFloorWeight", hardFloorWeight);

    // Old Generate
    //int numPoints = numPointsPerAxis * numPointsPerAxis * numPointsPerAxis; // Unused variable
    //int numThreadsPerAxis = std::round( 0.5f + numPointsPerAxis / static_cast<float>(threadGroupSize) ); // pour faire un ceil
    int numThreadsPerAxis = 1 + numPointsPerAxis / threadGroupSize; // pour faire un ceil
    // Points buffer is populated inside shader with pos (xyz) + density (w).
    // Set paramaters
    //densityShader.SetBuffer (0, "points", pointsBuffer);
    pointsBuffer.BindBase(1);

    densityShader.UniformVecF( "corner", 1, 3, &corner[0] ); // old ref did not work
    densityShader.UniformVecF( "offset", 1, 3, &offset[0] );
    densityShader.UniformVecF( "worldSize", 1, 3, &worldBounds[0] );

    densityShader.UniformI( "numPointsPerAxis", numPointsPerAxis );
    densityShader.UniformF( "boundsSize", boundsSize );
    densityShader.UniformF( "stride", spacing );

    // Dispatch shader
    densityShader.Dispatch(numThreadsPerAxis, numThreadsPerAxis, numThreadsPerAxis); opengl_debug();

    //if (buffersToRelease != null) {
    //    foreach (var b in buffersToRelease) {
    //        b.Release();
    //    }
    //}
    densityShader.MemBarrier(GL_ALL_BARRIER_BITS); opengl_debug();

    densityShader.Stop(); opengl_debug();

    // Return voxel data buffer so it can be used to generate mesh
    return pointsBuffer;
}



void MeshGeneratorBis::GenerateBuffer () {
    CreateBuffers (); opengl_debug();

    pointSSBO.Bind(); opengl_debug();
    pointSSBO.SetData(NULL,GL_STATIC_READ);

    densityGenerator.Generate(pointSSBO, numPointsPerAxis, boundsSize, worldBounds, cornerPoint, Pointoffset, stride); opengl_debug();
}



void MeshGeneratorBis::GenerateMesh () {
    int numThreadsPerAxis = std::round(0.5f + (numVoxelsPerAxis / static_cast<float>(threadGroupSize)));

    GLuint zero = 0;

    marching_shader.Run();

    pointSSBO.Bind();
    pointSSBO.BindBase(1);

    marchableCounter.BindBase(2);
    marchableCounter.SetData(&zero, GL_DYNAMIC_READ);

    pointCounter.BindBase(3);
    pointCounter.SetData(&zero, GL_DYNAMIC_READ);

    marchableSSBO.Bind();
    marchableSSBO.SetData(NULL,GL_DYNAMIC_READ);
    marchableSSBO.BindBase(4);

    //triangleBuffer.SetCounterValue (0);

    marching_shader.UniformI("numPointsPerAxis", numPointsPerAxis);
    marching_shader.UniformF("isoLevel", isoLevel);

    marching_shader.Dispatch (numThreadsPerAxis, numThreadsPerAxis, numThreadsPerAxis);
    marching_shader.MemBarrier(GL_ALL_BARRIER_BITS);

    GLuint marchCount;
    marchableCounter.Bind();
    marchableCounter.GetData(&marchCount);

    GLuint pointCount = 0;
    pointCounter.Bind();
    pointCounter.GetData(&pointCount);

    int jobSize = 256;
    int jobCount = marchCount/jobSize;
    if (marchCount%jobSize != 0)
    {
        jobCount++;

    }

    triangulation_shader.Run();

    triangulation_shader.UniformUI("marchableCount", marchCount);

    triangleCounter.BindBase(7);
    triangleCounter.SetData(&zero, GL_DYNAMIC_DRAW);

    vertexSSBO.BindBase(5);
    vertexSSBO.SetParams(pointCount, sizeof(vec4));
    vertexSSBO.SetData(NULL, GL_DYNAMIC_DRAW);

    // No of triangles : pointCount / 3 !!!
    triangleSSBO.BindBase(8);
    triangleSSBO.SetParams(int(pointCount/3), 4*sizeof(float));
    triangleSSBO.SetData(NULL, GL_DYNAMIC_DRAW);

    normalSSBO.BindBase(6);
    normalSSBO.SetParams(pointCount, sizeof(vec4));
    normalSSBO.SetData(NULL, GL_DYNAMIC_DRAW);

    triangulation_shader.UniformVecF( "corner", 1, 4, &vec4(cornerPoint.x, cornerPoint.y, cornerPoint.z, 1)[0] );
    triangulation_shader.UniformF( "stride", stride );


    triangulation_shader.Dispatch(jobCount, 1, 1);
    triangulation_shader.MemBarrier(GL_ALL_BARRIER_BITS);

    /*
    // Get number of triangles in the triangle buffer
    ComputeBuffer.CopyCount (triangleBuffer, triCountBuffer, 0);
    int[] triCountArray = { 0 };
    triCountBuffer.GetData (triCountArray);
    int numTris = triCountArray[0];

    // Get triangle data from shader
    Triangle[] tris = new Triangle[numTris];
    triangleBuffer.GetData (tris, 0, 0, numTris);
    */


    vertexSSBO.GetData(&vertices);
    //triangleSSBO.GetData(&triangles);
    // normalSSBO.GetData(normals);

    for (uint i = 0; i < pointCount/6; ++i) {
        //generator_mesh.connectivity.push_back(vec3(triangles[i].x, triangles[i].y, triangles[i].z));
        std::cout<<"1"<<std::endl;
        generator_mesh.position.push_back(vec3(vertices[3*i].x, vertices[3*i].y, vertices[3*i].z));
        std::cout<<"2"<<std::endl;
        generator_mesh.position.push_back(vec3(vertices[3*i+1].x, vertices[3*i+1].y, vertices[3*i+1].z));
        std::cout<<"3"<<std::endl;
        generator_mesh.position.push_back(vec3(vertices[3*i+2].x, vertices[3*i+2].y, vertices[3*i+2].z));
        std::cout<<"4"<<std::endl;
        generator_mesh.connectivity.push_back({3*i, 3*i+1, 3*i+2});
        std::cout<<"5"<<std::endl;
    }

    //mesh.RecalculateNormals ();
}

/*
void MeshGenerator::OnDestroy () {
    if (Application.isPlaying) {
        ReleaseBuffers ();
    }
}
*/



/*
void MeshGenerator::ReleaseBuffers () {
    if (triangleBuffer != null) {
        triangleBuffer.Release ();
        pointsBuffer.Release ();
        triCountBuffer.Release ();
    }
}
*/

/*
vec3 MeshGenerator::CentreFromCoord (vec3 coord) {
    // Centre entire map at origin
    if (fixedMapSize) {
        vec3 totalBounds = vec3(numChunks * boundsSize);
        return -totalBounds / 2 + coord * boundsSize + vec3(1,1,1) * boundsSize / 2;
    }

    return vec3(coord.x, coord.y, coord.z) * boundsSize;
}
*/

/*
void MeshGenerator::OnDrawGizmos () {
    if (showBoundsGizmo) {
        Gizmos.color = boundsGizmoCol;

        foreach (var chunk in chunks) {
            Bounds bounds = new Bounds (CentreFromCoord (chunk.coord), Vector3.one * boundsSize);
            Gizmos.color = boundsGizmoCol;
            Gizmos.DrawWireCube (CentreFromCoord (chunk.coord), Vector3.one * boundsSize);
        }
    }
}
*/

