
#include "terrain_creation.hpp"


using namespace vcl;



//-----------------------------------------------------------------------------
// heightmap_generator methods
//-----------------------------------------------------------------------------

vcl::buffer<float> mesh_generator::Generate_Heightmap_CPU(int mapSizeValue)
{
    std::random_device                  rand_dev;
    std::mt19937                        generator_unseeded(rand_dev());
    std::uniform_int_distribution<int>  distr_seed(-10000, 10000);
    std::uniform_int_distribution<int>  distr_prng(-1000, 1000);

    heightmap_seed = (randomizeSeed) ? distr_seed(generator_unseeded) : heightmap_seed;
    //var prng = new System.Random (seed);
    std::mt19937                        prng(heightmap_seed);

    vcl::buffer<float> return_map;    // size mapsize*mapsize
    return_map.resize(mapSizeValue*mapSizeValue);

    buffer<vec2> offsets; // size numOctaves
    for (int i = 0; i < numOctaves; ++i) {
        int ox = distr_prng(prng);
        int oy = distr_prng(prng);

        offsets.push_back(vec2(static_cast<float>(ox), static_cast<float>(oy)));
    }

    float minValue = FLT_MAX;
    float maxValue = -FLT_MAX;

    for (int y = 0; y < mapSizeValue; ++y) {
        for (int x = 0; x < mapSizeValue; ++x) {
            float noiseValue = 0;
            float scale = initialScale;
            float weight = 1;
            for (int i = 0; i < numOctaves; i++) {
                vec2 p = offsets[i] + vec2 (x / static_cast<float>(mapSizeValue), y / static_cast<float>(mapSizeValue)) * scale;
                noiseValue += perlin(p.x, p.y, 1) * weight;
                weight *= persistence;
                scale *= lacunarity;
            }
            return_map[y * mapSizeValue + x] = noiseValue;
            minValue = std::min (noiseValue, minValue);
            maxValue = std::max (noiseValue, maxValue);
        }
    }

    // Normalize
    if ((maxValue - minValue) < 1e-6f) {
        for (size_t i = 0; i < return_map.size(); ++i) {
            return_map[i] = (return_map[i] - minValue) / (maxValue - minValue);
        }
    }

    return return_map;
}


vcl::buffer<float> mesh_generator::Generate_Heightmap_GPU(int mapSizeValue)
{
    std::random_device                  rand_dev;
    std::mt19937                        generator_unseeded(rand_dev());
    std::uniform_int_distribution<int>  distr_seed(-10000, 10000);
    std::uniform_int_distribution<int>  distr_prng(-1000, 1000);

    heightmap_seed = (randomizeSeed) ? distr_seed(generator_unseeded) : heightmap_seed;
    //var prng = new System.Random (seed);
    std::mt19937                        prng(heightmap_seed);

    vcl::buffer<float> return_map;    // size mapsize*mapsize
    return_map.resize(mapSizeValue*mapSizeValue);

    buffer<vec2> offsets; // size numOctaves

    for (int i = 0; i < numOctaves; ++i) {
        int ox = distr_prng(prng);
        int oy = distr_prng(prng);

        offsets.push_back(vec2(static_cast<float>(ox), static_cast<float>(oy)));
    }

    glUseProgram(heightmap_program);

    int floatToIntMultiplier = 1000;

    buffer_stack<int,2> minmax = { floatToIntMultiplier * numOctaves, 0 };

    glGenBuffers(1, &offsets_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, offsets_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, offsets_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec2)*offsets.size(), &offsets[0], GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &map_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, map_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, map_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float)*return_map.size(), &return_map[0], GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &minmax_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, minmax_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, minmax_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int)*minmax.size(), &minmax[0], GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glUseProgram(heightmap_program);

    glUniform1i(glGetUniformLocation(heightmap_program, "floatToIntMultiplier"), floatToIntMultiplier);
    glUniform1i(glGetUniformLocation(heightmap_program, "mapSize"), mapSizeValue);
    glUniform1i(glGetUniformLocation(heightmap_program, "octaves"), numOctaves);
    glUniform1f(glGetUniformLocation(heightmap_program, "lacunarity"), lacunarity);
    glUniform1f(glGetUniformLocation(heightmap_program, "persistence"), persistence);
    glUniform1f(glGetUniformLocation(heightmap_program, "scaleFactor"), initialScale);

    glDispatchCompute(mapSizeValue*mapSizeValue, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    /*
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, map_ssbo);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float)*map.size(), &map);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    */

    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, map_ssbo);
    //float * map_data = static_cast<float *>(glMapBufferRange(GL_SHADER_STORAGE_BLOCK, 0, sizeof(float)*return_map.size(), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT));

    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, minmax_ssbo);
    //int * minmax_data = static_cast<int *>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int)*minmax.size(), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT));
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glUseProgram(0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, map_ssbo);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float)*return_map.size(), &return_map[0]);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, minmax_ssbo);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int)*minmax.size(), &minmax[0]);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Normalize
    float minValue = static_cast<float>(minmax[0]) / static_cast<float>(floatToIntMultiplier);
    float maxValue = static_cast<float>(minmax[1]) / static_cast<float>(floatToIntMultiplier);

    std::cout<<"minValue : "<<minValue<<" maxValue : "<<maxValue<<std::endl;

    for (int i = 0; i < mapSizeValue*mapSizeValue; ++i) {
        return_map[i] = (return_map[i] - minValue) / (maxValue - minValue);
    }

    //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    //glDeleteBuffers(1, &nameofBuffer);

    return return_map;
}



//-----------------------------------------------------------------------------
// erosion_generator methods
//-----------------------------------------------------------------------------
/*
void erosion_generator::InitializeBrushIndices (int mapSize, int radius)
{
    erosionBrushIndices.resize(mapSize*mapSize);
    erosionBrushWeights.resize(mapSize*mapSize);

    buffer<int> xOffsets;        // size radius*radius*4
    xOffsets.resize(radius*radius*4);
    buffer<int> yOffsets;        // size radius*radius*4
    yOffsets.resize(radius*radius*4);
    buffer<float> weights;       // size radius*radius*4
    weights.resize(radius*radius*4);
    float weightSum = 0;
    int addIndex = 0;

    for (size_t i = 0; i < erosionBrushIndices.size(); ++i) {
        int centreX = i % mapSize;
        int centreY = i / mapSize;

        if (centreY <= radius || centreY >= mapSize - radius || centreX <= radius + 1 || centreX >= mapSize - radius) {
            weightSum = 0;
            addIndex = 0;
            for (int y = -radius; y <= radius; ++y) {
                for (int x = -radius; x <= radius; ++x) {
                    float sqrDst = x * x + y * y;
                    if (sqrDst < radius * radius) {
                        int coordX = centreX + x;
                        int coordY = centreY + y;

                        if (coordX >= 0 && coordX < mapSize && coordY >= 0 && coordY < mapSize) {
                            float weight = 1 - std::sqrt (sqrDst) / radius;
                            weightSum += weight;
                            weights[addIndex] = weight;
                            xOffsets[addIndex] = x;
                            yOffsets[addIndex] = y;
                            addIndex++;
                        }
                    }
                }
            }
        }

        int numEntries = addIndex;

        erosionBrushIndices[i].resize(numEntries);
        erosionBrushWeights[i].resize(numEntries);

        for (int j = 0; j < numEntries; j++) {
            erosionBrushIndices[i][j] = (yOffsets[j] + centreY) * mapSize + xOffsets[j] + centreX;
            erosionBrushWeights[i][j] = weights[j] / weightSum;
        }
    }
}


// Initialization creates a System.Random object and precomputes indices and weights of erosion brush
void erosion_generator::Initialize (int mapSize) {
    prng_2.seed(seed);
    //std::uniform_real_distribution<> float_distr(0, mapSize-1); // 0 to mapSize - 1
    currentSeed = seed;

    InitializeBrushIndices(mapSize, erosionRadius);
    currentErosionRadius = erosionRadius;
    currentMapSize = mapSize;
}


void erosion_generator::UpdateRNG (int mapSize, bool resetSeed) {
    //std::uniform_real_distribution<> distr(0, mapSize-1);
    if (resetSeed || currentSeed != seed) {
        prng_2.seed(seed);
        currentSeed = seed;
    }
    if (currentErosionRadius != erosionRadius || currentMapSize != mapSize) {
        InitializeBrushIndices(mapSize, erosionRadius);
        currentErosionRadius = erosionRadius;
        currentMapSize = mapSize;
    }
}


vec3 erosion_generator::CalculateHeightAndGradient (buffer<float>& nodes, int mapSize, float posX, float posY) {
    int coordX = static_cast<int>(posX);
    int coordY = static_cast<int>(posY);
    //std::cout<<"at pos gradient : "<<posX<<" "<<posY<<std::endl;

    // Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
    float x = posX - coordX;
    float y = posY - coordY;

    // Calculate heights of the four nodes of the droplet's cell
    int nodeIndexNW = coordY * mapSize + coordX;

    //std::cout<<"gradient map["<<nodeIndexNW<<"] value : "<<nodes[nodeIndexNW]<<std::endl;
    float heightNW = nodes[nodeIndexNW];
    //std::cout<<"gradient map["<<nodeIndexNW+1<<"] value : "<<nodes[nodeIndexNW+1]<<std::endl;
    float heightNE = nodes[nodeIndexNW + 1];
    //std::cout<<"gradient map["<<nodeIndexNW+mapSize<<"] value : "<<nodes[nodeIndexNW+mapSize]<<std::endl;
    float heightSW = nodes[nodeIndexNW + mapSize];
    //std::cout<<"gradient map["<<nodeIndexNW+mapSize+1<<"] value : "<<nodes[nodeIndexNW+mapSize+1]<<std::endl;
    float heightSE = nodes[nodeIndexNW + mapSize + 1];

    // Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
    float gradientX = (heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y;
    float gradientY = (heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x;

    // Calculate height with bilinear interpolation of the heights of the nodes of the cell
    float height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;

    //std::cout<<"final gradient : "<<height<<" "<<gradientX<<" "<<gradientY<<std::endl;
    return vec3(height, gradientX, gradientY); // est-ce que ca renvoie le bon objet?
}


void erosion_generator::Erode (buffer<float>& map, int mapSize, int numIterations, bool resetSeed) {
    UpdateRNG(mapSize, resetSeed);

    for (int iteration = 0; iteration < numIterations; iteration++) {
        // Create water droplet at random point on map
        std::uniform_real_distribution<> float_distr(0, mapSize-1);
        float posX = float_distr(prng_2);
        float posY = float_distr(prng_2);

        float dirX = 0;
        float dirY = 0;
        float speed = initialSpeed;
        float water = initialWaterVolume;
        float sediment = 0;

        //std::cout<<"--------------------------------"<<std::endl;
        //std::cout<<"          droplet init          "<<std::endl;
        //std::cout<<"--------------------------------"<<std::endl;

        for (int lifetime = 0; lifetime < maxDropletLifetime; ++lifetime) {
            int nodeX = static_cast<int>(posX);
            int nodeY = static_cast<int>(posY);
            int dropletIndex = nodeY * mapSize + nodeX;
            // Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
            float cellOffsetX = posX - nodeX;
            float cellOffsetY = posY - nodeY;

            // Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
            vec3 heightAndGradient = CalculateHeightAndGradient(map, mapSize, posX, posY);
            //std::cout<<lifetime<<" iteration, first gradient : "<<heightAndGradient<<std::endl;

            // Update the droplet's direction and position (move position 1 unit regardless of speed)
            dirX = (dirX * inertia - heightAndGradient.y * (1 - inertia));
            dirY = (dirY * inertia - heightAndGradient.z * (1 - inertia));
            // Normalize direction
            float len = std::sqrt (dirX * dirX + dirY * dirY);
            if (len > 1e-6f) {
                dirX /= len;
                dirY /= len;
            }
            posX += dirX;
            posY += dirY;

            // Stop simulating droplet if it's not moving or has flowed over edge of map
            if ((dirX < 1e-6f && dirY < 1e-6f) || posX < 0 || posX >= mapSize - 1 || posY < 0 || posY >= mapSize - 1) {
                break;
            }

            // Find the droplet's new height and calculate the deltaHeight
            float newHeight = CalculateHeightAndGradient(map, mapSize, posX, posY).x;

            float deltaHeight = newHeight - heightAndGradient.x;

            // Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
            float sedimentCapacity = std::max(-deltaHeight * speed * water * sedimentCapacityFactor, minSedimentCapacity);
            //std::cout<<"speed :"<<speed<<", water : "<<water<<", sedCapFactor : "<<sedimentCapacityFactor<<", minSedCap : "<<minSedimentCapacity<<", dH : "<<deltaHeight<<std::endl;

            // If carrying more sediment than capacity, or if flowing uphill:
            if (sediment > sedimentCapacity || deltaHeight > 0) {
                // If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
                float amountToDeposit = (deltaHeight > 0) ? std::min (deltaHeight, sediment) : (sediment - sedimentCapacity) * depositSpeed;
                sediment -= amountToDeposit;

                // Add the sediment to the four nodes of the current cell using bilinear interpolation
                // Deposition is not distributed over a radius (like erosion) so that it can fill small pits
                map[dropletIndex] += amountToDeposit * (1 - cellOffsetX) * (1 - cellOffsetY);
                map[dropletIndex + 1] += amountToDeposit * cellOffsetX * (1 - cellOffsetY);
                map[dropletIndex + mapSize] += amountToDeposit * (1 - cellOffsetX) * cellOffsetY;
                //std::cout<<"adding sediment at map["<<dropletIndex+mapSize+1<<"] value : "<<amountToDeposit * cellOffsetX * cellOffsetY<<std::endl;
                map[dropletIndex + mapSize + 1] += amountToDeposit * cellOffsetX * cellOffsetY;

            } else {
                // Erode a fraction of the droplet's current carry capacity.
                // Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
                float amountToErode = std::min((sedimentCapacity - sediment) * erodeSpeed, -deltaHeight);
                //std::cout<<"sedCap :"<<sedimentCapacity<<", sediment : "<<sediment<<", erodeSpeed : "<<erodeSpeed<<", dH : "<<deltaHeight<<std::endl;

                // Use erosion brush to erode from all nodes inside the droplet's erosion radius
                for (size_t brushPointIndex = 0; brushPointIndex < erosionBrushIndices[dropletIndex].size(); ++brushPointIndex) {
                    int nodeIndex = erosionBrushIndices[dropletIndex][brushPointIndex];
                    //if (nodeIndex == 255*128+128)
                    //    std::cout<<amountToErode<<std::endl;
                    float weighedErodeAmount = amountToErode * erosionBrushWeights[dropletIndex][brushPointIndex];
                    float deltaSediment = (map[nodeIndex] < weighedErodeAmount) ? map[nodeIndex] : weighedErodeAmount;
                    map[nodeIndex] -= deltaSediment;
                    sediment += deltaSediment;
                }
            }

            // Update droplet's speed and water content
            speed = std::sqrt( std::max<float>(0, speed * speed + deltaHeight * gravity) );
            // have to specify template because max cannot deduce it by itself when different types
            water *= (1 - evaporateSpeed);
        }
    }
}
*/


//-----------------------------------------------------------------------------
// mesh_generator methods
//-----------------------------------------------------------------------------

//void mesh_generator::Start () {
//    StartMeshGeneration();
//    erosion = FindObjectOfType<Erosion> ();
//    //Application.runInBackground = true;
//}

/*
void mesh_generator::StartMeshGeneration() {
    map = heightmap.Generate_CPU(mapSize);
    erosion.Initialize(mapSize);
    GenerateMesh();
}
*/

/*
void mesh_generator::Erode() {
    map = heightmap.Generate_GPU(mapSize); // map = heightmap.Generate_CPU(mapSize);
    erosion.Initialize(mapSize);
    erosion.Erode(map, mapSize, numErosionIterations, true);
    GenerateMesh();
}
*/


void mesh_generator::Start_Map() {
    std::cout<<"mapSize : "<< mapSize <<std::endl;
    std::cout<<"scale : "<< scale<<std::endl;
    std::cout<<"elevationScale : "<< elevationScale<<std::endl;
    std::cout<<"numErosionIterations : "<< numErosionIterations<<std::endl;
    std::cout<<"animateErosion : "<< animateErosion<<std::endl;
    std::cout<<"iterationsPerFrame : "<< iterationsPerFrame<<std::endl;
    std::cout<<"showNumIterations : "<< showNumIterations<<std::endl;
    std::cout<<"numAnimatedErosionIterations : "<< numAnimatedErosionIterations<<std::endl;

    std::cout<<"seed : "<<erosion_seed<<std::endl;

    mapSizeWithBorder = mapSize + erosionBrushRadius * 2;
    std::cout<<"mapSize with border : "<<mapSizeWithBorder<<std::endl;
    map.resize(mapSizeWithBorder*mapSizeWithBorder);

    std::fill(map.begin(), map.end(), 0);
}


void mesh_generator::Heightmap_GPU() {
    map = Generate_Heightmap_GPU(mapSizeWithBorder);
}


void mesh_generator::Heightmap_CPU() {
    map = Generate_Heightmap_CPU(mapSizeWithBorder);
}


void mesh_generator::Erode_GPU()
{
    int numThreads = numErosionIterations / 1024;

    std::mt19937 prng;
    std::uniform_int_distribution<> int_distr(erosionBrushRadius, mapSize + erosionBrushRadius);

    prng.seed(erosion_seed);

    // Create brush
    buffer<int> brushIndexOffsets;
    buffer<float> brushWeights;

    float weightSum = 0;
    for (int brushY = -erosionBrushRadius; brushY <= erosionBrushRadius; brushY++) {
        for (int brushX = -erosionBrushRadius; brushX <= erosionBrushRadius; brushX++) {
            float sqrDst = brushX * brushX + brushY * brushY;
            if (sqrDst < erosionBrushRadius * erosionBrushRadius) {
                brushIndexOffsets.push_back(brushY * mapSize + brushX);
                float brushWeight = 1 - std::sqrt (sqrDst) / erosionBrushRadius;
                weightSum += brushWeight;
                brushWeights.push_back(brushWeight);
            }
        }
    }

    for (size_t i = 0; i < brushWeights.size(); ++i) {
        brushWeights[i] /= weightSum;
    }

    // Generate random indices for droplet placement
    buffer<int> randomIndices;
    randomIndices.resize(numErosionIterations);

    for (int i = 0; i < numErosionIterations; ++i) {
        int randomX = int_distr(prng);
        int randomY = int_distr(prng);
        randomIndices[i] = randomY * mapSize + randomX;
    }

    // Send brush data to compute shader
    glGenBuffers(1, &brushIndexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, brushIndexBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int)*brushIndexOffsets.size(), &brushIndexOffsets, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, brushIndexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &brushWeightBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, brushWeightBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int)*brushWeights.size(), &brushWeights, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, brushWeightBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Send random indices to compute shader
    glGenBuffers(1, &randomIndexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, randomIndexBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int)*randomIndices.size(), &randomIndices[0], GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, randomIndexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Heightmap buffer
    glGenBuffers(1, &mapBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mapBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float)*map.size(), &map[0], GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, mapBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glUseProgram(erosion_program);

    // Settings
    glUniform1i(glGetUniformLocation(erosion_program, "borderSize"), erosionBrushRadius);
    glUniform1i(glGetUniformLocation(erosion_program, "mapSize"), mapSizeWithBorder);
    glUniform1i(glGetUniformLocation(erosion_program, "brushLength"), brushIndexOffsets.size());
    glUniform1i(glGetUniformLocation(erosion_program, "maxLifetime"), maxLifetime);

    glUniform1f(glGetUniformLocation(erosion_program, "inertia"), inertia);
    glUniform1f(glGetUniformLocation(erosion_program, "sedimentCapacityFactor"), sedimentCapacityFactor);
    glUniform1f(glGetUniformLocation(erosion_program, "minSedimentCapacity"), minSedimentCapacity);
    glUniform1f(glGetUniformLocation(erosion_program, "depositSpeed"), depositSpeed);
    glUniform1f(glGetUniformLocation(erosion_program, "erodeSpeed"), erodeSpeed);
    glUniform1f(glGetUniformLocation(erosion_program, "evaporateSpeed"), evaporateSpeed);
    glUniform1f(glGetUniformLocation(erosion_program, "gravity"), gravity);
    glUniform1f(glGetUniformLocation(erosion_program, "startSpeed"), startSpeed);
    glUniform1f(glGetUniformLocation(erosion_program, "startWater"), startWater);

    // Run compute shader
    glDispatchCompute(numThreads, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glUseProgram(0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mapBuffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float)*map.size(), &map[0]);

    // doesn't do shit
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, mapBuffer);
    //float * map_data = (float *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float)*map.size(), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    //float * map_data = (float *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float)*mapSizeWithBorder*mapSizeWithBorder, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    /*
    std::cout<<"map before : "<<map[mapSizeWithBorder + 6]<<std::endl;

    for (int i=0; i<mapSizeWithBorder*mapSizeWithBorder; ++i)
        map[i] = map_data[i];

    std::cout<<"map after : "<<map[mapSizeWithBorder + 6]<<std::endl;
    */
    // Release buffers


}

vec3 mesh_generator::CalculateHeightAndGradient (buffer<float>& nodes, int mapSize, float posX, float posY) {
    int coordX = static_cast<int>(posX);
    int coordY = static_cast<int>(posY);
    //std::cout<<"at pos gradient : "<<posX<<" "<<posY<<std::endl;

    // Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
    float x = posX - coordX;
    float y = posY - coordY;

    // Calculate heights of the four nodes of the droplet's cell
    int nodeIndexNW = coordY * mapSize + coordX;

    //std::cout<<"gradient map["<<nodeIndexNW<<"] value : "<<nodes[nodeIndexNW]<<std::endl;
    float heightNW = nodes[nodeIndexNW];
    //std::cout<<"gradient map["<<nodeIndexNW+1<<"] value : "<<nodes[nodeIndexNW+1]<<std::endl;
    float heightNE = nodes[nodeIndexNW + 1];
    //std::cout<<"gradient map["<<nodeIndexNW+mapSize<<"] value : "<<nodes[nodeIndexNW+mapSize]<<std::endl;
    float heightSW = nodes[nodeIndexNW + mapSize];
    //std::cout<<"gradient map["<<nodeIndexNW+mapSize+1<<"] value : "<<nodes[nodeIndexNW+mapSize+1]<<std::endl;
    float heightSE = nodes[nodeIndexNW + mapSize + 1];

    // Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
    float gradientX = (heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y;
    float gradientY = (heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x;

    // Calculate height with bilinear interpolation of the heights of the nodes of the cell
    float height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;

    //std::cout<<"final gradient : "<<height<<" "<<gradientX<<" "<<gradientY<<std::endl;
    return vec3(gradientX, gradientY, height); // est-ce que ca renvoie le bon objet?
}


void mesh_generator::Erode_CPU() {
    std::mt19937 prng;
    std::uniform_real_distribution<> real_distr(erosionBrushRadius, mapSize + erosionBrushRadius);
    prng.seed(erosion_seed);

    // Create brush
    buffer<int> brushIndexOffsets;
    buffer<float> brushWeights;

    float weightSum = 0;
    for (int brushY = -erosionBrushRadius; brushY <= erosionBrushRadius; brushY++) {
        for (int brushX = -erosionBrushRadius; brushX <= erosionBrushRadius; brushX++) {
            float sqrDst = brushX * brushX + brushY * brushY;
            if (sqrDst < erosionBrushRadius * erosionBrushRadius) {
                brushIndexOffsets.push_back(brushY * mapSize + brushX);
                float brushWeight = 1 - std::sqrt (sqrDst) / erosionBrushRadius;
                weightSum += brushWeight;
                brushWeights.push_back(brushWeight);
            }
        }
    }

    for (size_t i = 0; i < brushWeights.size(); ++i) {
        brushWeights[i] /= weightSum;
    }

    for (int iteration = 0; iteration < numErosionIterations; ++iteration)
    {
        float posX = real_distr(prng);
        float posY = real_distr(prng);
        float dirX = 0;
        float dirY = 0;
        float speed = startSpeed;
        float water = startWater;
        float sediment = 0;

        for (int lifetime = 0; lifetime < maxLifetime; ++lifetime) {
            int nodeX = static_cast<int>(posX);
            int nodeY = static_cast<int>(posY);
            int dropletIndex = nodeY * mapSizeWithBorder + nodeX;
            // Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
            float cellOffsetX = posX - nodeX;
            float cellOffsetY = posY - nodeY;

            // Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
            vec3 heightAndGradient = CalculateHeightAndGradient(map, mapSizeWithBorder, posX, posY);

            // Update the droplet's direction and position (move position 1 unit regardless of speed)
            dirX = (dirX * inertia - heightAndGradient.x * (1 - inertia));
            dirY = (dirY * inertia - heightAndGradient.y * (1 - inertia));
            // Normalize direction
            float len = std::max(0.01f,std::sqrt(dirX * dirX + dirY * dirY));
            dirX /= len;
            dirY /= len;
            posX += dirX;
            posY += dirY;

            // Stop simulating droplet if it's not moving or has flowed over edge of map
            if ((dirX < 1e-6f && dirY < 1e-6f) || posX < erosionBrushRadius || posX > mapSizeWithBorder - erosionBrushRadius || posY < erosionBrushRadius || posY > mapSizeWithBorder - erosionBrushRadius) {
                break;
            }

            // Find the droplet's new height and calculate the deltaHeight
            float newHeight = CalculateHeightAndGradient (map, mapSizeWithBorder, posX, posY).z;
            float deltaHeight = newHeight - heightAndGradient.z;

            // Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
            float sedimentCapacity = std::max(-deltaHeight * speed * water * sedimentCapacityFactor, minSedimentCapacity);

            // If carrying more sediment than capacity, or if flowing uphill:
            if (sediment > sedimentCapacity || deltaHeight > 0) {
                // If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
                float amountToDeposit = (deltaHeight > 0) ? std::min(deltaHeight, sediment) : (sediment - sedimentCapacity) * depositSpeed;
                sediment -= amountToDeposit;

                // Add the sediment to the four nodes of the current cell using bilinear interpolation
                // Deposition is not distributed over a radius (like erosion) so that it can fill small pits
                map[dropletIndex] += amountToDeposit * (1 - cellOffsetX) * (1 - cellOffsetY);
                map[dropletIndex + 1] += amountToDeposit * cellOffsetX * (1 - cellOffsetY);
                map[dropletIndex + mapSizeWithBorder] += amountToDeposit * (1 - cellOffsetX) * cellOffsetY;
                map[dropletIndex + mapSizeWithBorder + 1] += amountToDeposit * cellOffsetX * cellOffsetY;

            }
            else {
                // Erode a fraction of the droplet's current carry capacity.
                // Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet

                float amountToErode = std::min((sedimentCapacity - sediment) * erodeSpeed, -deltaHeight);

                for (size_t i = 0; i < brushIndexOffsets.size(); ++i) {
                    int erodeIndex = dropletIndex + brushIndexOffsets[i];
                    float weightedErodeAmount = amountToErode * brushWeights[i];
                    float deltaSediment = (map[erodeIndex] < weightedErodeAmount) ? map[erodeIndex] : weightedErodeAmount;
                    map[erodeIndex] -= deltaSediment;
                    sediment += deltaSediment;
                }
            }

            // Update droplet's speed and water content
            speed = std::sqrt( std::max<float>(0, speed * speed + deltaHeight * gravity) );
            // have to specify template because max cannot deduce it by itself when different types
            water *= (1 - evaporateSpeed);
        }
    }
}



/*
void mesh_generator::UpdateErosion() {
    if (animateErosion) {
        for (int i = 0; i < iterationsPerFrame; i++) {
            erosion.Erode(map, mapSize);
        }
        numAnimatedErosionIterations += iterationsPerFrame;
        UpdateMesh();
    }
}
*/


void mesh_generator::GenerateMesh() {
    buffer<vec3> verts; // size mapsize*mapsize
    verts.resize(mapSizeWithBorder*mapSizeWithBorder);
    buffer<uint3> triangles; // = new int[(mapSize - 1) * (mapSize - 1) * 6];   en fait pour mesh la connectivity est pas pareil
    triangles.resize((mapSizeWithBorder - 1) * (mapSizeWithBorder - 1) * 2);

    unsigned int i = 0;
    int t = 0;

    for (int y = 0; y < mapSizeWithBorder; ++y) {
        for (int x = 0; x < mapSizeWithBorder; ++x) {
            // unsigned int i = y * mapSize + x;

            vec2 percent = vec2(x / (mapSizeWithBorder - 1.f), y / (mapSizeWithBorder - 1.f));     //1f in C#
            vec3 pos = vec3(percent.x * 2 - 1, percent.y * 2 - 1, 0);
            pos *= scale;
            pos += vec3(0,0,1) * map[i] * elevationScale;
            verts[i] = pos;

            // Construct triangles
            if (x != mapSizeWithBorder - 1 && y != mapSizeWithBorder - 1) {

                triangles[t] = {i+mapSizeWithBorder, i, i+mapSizeWithBorder+1};
                triangles[t+1] = {i+mapSizeWithBorder+1, i, i+1};

                t += 2;
            }

            ++i;
        }
    }

    mesh.position = verts;
    mesh.connectivity = triangles;
    //mesh.RecalculateNormals ();
}


void mesh_generator::UpdateMesh() {
    for (int y = 0; y < mapSizeWithBorder; ++y) {
        for (int x = 0; x < mapSizeWithBorder; ++x) {
            unsigned int i = y * mapSizeWithBorder + x;

            vec2 percent = vec2(x / (mapSizeWithBorder - 1.f), y / (mapSizeWithBorder - 1.f));     //1f in C#
            vec3 pos = vec3(percent.x * 2 - 1, percent.y * 2 - 1, 0);
            pos *= scale;
            pos += vec3(0,0,1) * map[i] * elevationScale;
            mesh.position[i] = pos;
        }
    }

    mesh.normal = normal(mesh.position, mesh.connectivity);
}



