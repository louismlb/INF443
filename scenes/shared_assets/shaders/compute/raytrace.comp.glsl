#version 430

#extension GL_NV_shadow_samplers_cube : enable

layout (local_size_x = 1, local_size_y = 1) in;

layout (rgba32f, binding = 0) uniform image2D img_output;
// or writeonly uniform image2D img_output;

layout (binding = 1) uniform samplerCube skybox;

const float infinity = 1. / 0.;

const float PI = 3.14159265f;

uniform mat4 view;
uniform mat4 perspective;
uniform vec3 camera_position;


mat4 _CameraToWorld = inverse(view);
mat4 _CameraInverseProjection = inverse(perspective);

// Can use #include "common/shader.glsl" to split it

//-------------------------------------
//- UTILITY

float sdot(vec3 x, vec3 y, float f = 1.0f)
{
    return clamp(dot(x, y) * f, 0.0, 1.0);
}

float energy(vec3 color)
{
    return (color.x+color.y+color.z) * 1.0f / 3.0f;
}

//-------------------------------------
//- RANDOMNESS

float noise(vec2 co, float seed)
{
    return fract(sin(seed/100 * dot(co ,vec2(12.9898,78.233))) * 43758.5453);
}


float _Seed = 0;

float rand(vec2 co, float scale, inout float seed)
{
    vec2 pos = co;
    ivec2 ipos = ivec2(co / scale);
    pos = vec2(ipos) * scale;
    float r = noise(pos, seed);
    seed += 1.f;
    return r;
}

float rand()
{
    return rand(vec2(1,1),1,_Seed);
}


/*
vec2 seed;
//-----------------------------------------------------------------------
float rand()
//-----------------------------------------------------------------------
{
        seed -= vec2(randomVector.x * randomVector.y);
        return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
} // randomVector should be sent as uniform?
*/


struct Sphere {
    vec3 position;
    float radius;
    //vec4 color;

    vec3 albedo;
    float smoothness;

    vec3 specular;
    float foo; //padding, not necessary in glsl?

    vec3 emission;
    float bar; //padding, not necessary?
};

struct Plane {
    vec3 position;
    vec3 normal;
    float size;
};

/*
Sphere SphereArr.spheres[4] = {
{vec3(0.0,0.0,1.0),0.5,vec4(0.8, 0.8, 0.8, 1.0)},
{vec3(5.0,0.0,1.0),0.5,vec4(1.0, 0.0, 0.0, 1.0)},
{vec3(0.0,5.0,1.0),0.5,vec4(0.0, 1.0, 0.0, 1.0)},
{vec3(0.0,0.0,3.0),0.5,vec4(0.0, 0.0, 1.0, 1.0)}
};
*/

/*
Sphere SphereArr.spheres[2] = {
{vec3(0.0,0.0,1.0), 0.5, vec3(0.8, 0.1, 0.1), vec3(.2,.2,.2), 0.2, vec3(1,1,1)},
{vec3(2.0,1.0,1.0), 0.5, vec3(0.1, 0.8, 0.1), vec3(.2,.2,.2), 0.4, vec3(0,0,0)}
};
*/

/* Uniform version, faster, have to replace GL_SHADER_STORAGE_BUFFER with GL_UNIFORM_BUFFER
layout(binding = 0) uniform SphereArray {
    Sphere[4] spheres;
} SphereArr;
*/

layout(std430, binding=0) buffer SphereArray {
    Sphere[4] spheres;
} SphereArr;

Plane groundplane = Plane(vec3(0,0,0),vec3(0,0,1), 4);


struct Ray
{
    vec3 origin;
    vec3 direction;
    vec3 energy;
};

Ray CreateRay(vec3 origin, vec3 direction)
{
    Ray ray;
    ray.origin = origin;
    ray.direction = direction;
    ray.energy = vec3(1.0f, 1.0f, 1.0f);
    return ray;
}

Ray CreateCameraRay(vec2 uv)
{
    // Transform the camera origin to world space
    vec3 origin = camera_position; //(_CameraToWorld * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;

    // Invert the perspective projection of the view-space position
    vec3 direction = (_CameraInverseProjection * vec4(uv, 0.0f, 1.0f)).xyz;
    // Transform the direction from camera to world space and normalize
    direction = (_CameraToWorld * vec4(direction, 0.0f)).xyz;
    direction = normalize(direction);

    // Maybe the directions are not right, thus having to rotate cubemap. Should also check GL cubemap faces Images : yes, they are mixed up

    return CreateRay(origin, direction);
}

struct RayHit
{
    vec3 position;
    float distance;
    vec3 normal;
    //vec4 color;

    vec3 albedo;
    vec3 specular;
    float smoothness;
    vec3 emission;
};

RayHit CreateRayHit()
{
    RayHit hit;
    hit.position = vec3(0.0f, 0.0f, 0.0f);
    hit.distance = infinity;  //1.#INF
    hit.normal = vec3(0.0f, 0.0f, 0.0f);
    hit.albedo = vec3(0.0f, 0.0f, 0.0f);
    hit.specular = vec3(0.0f, 0.0f, 0.0f);
    hit.smoothness = 0.0f;
    hit.emission = vec3(0.0f, 0.0f, 0.0f);

    return hit;
}

void IntersectPlane(Ray ray, inout RayHit bestHit, Plane plane)
{
    // Calculate distance along the ray where the ground plane is intersected
    //float t = -ray.origin.z / ray.direction.z;
    vec3 p0 = plane.position;
    vec3 n = plane.normal;
    float size = plane.size;

    float t = - dot(ray.origin-p0, n)/dot(ray.direction,n);

    if (t > 0 && t < bestHit.distance)
    {
        if (max(abs((ray.origin + t*ray.direction).x), abs((ray.origin + t*ray.direction).y)) > size/2) // only for horizontal, should change to cross prod with n
            return;
        bestHit.distance = t;
        bestHit.position = ray.origin + t * ray.direction;
        bestHit.normal = vec3(0.0f, 0.0f, 1.0f);
        //bestHit.color = vec4(.8f,.2f,.2f,1.f);

        bestHit.albedo = vec3(0.5f);  // 0.5f
        bestHit.specular = vec3(0.03f);  // 0.03f
        bestHit.smoothness = 0.2f;
        bestHit.emission = vec3(0.0f, 0.0f, 0.0f);
    }
}

void IntersectSphere(Ray ray, inout RayHit bestHit, Sphere sphere)
{
    // Calculate distance along the ray where the sphere is intersected
    vec3 d = ray.origin - sphere.position;
    float p1 = -dot(ray.direction, d);
    float p2sqr = p1 * p1 - dot(d, d) + sphere.radius * sphere.radius;
    if (p2sqr < 0)
        return;
    float p2 = sqrt(p2sqr);
    float t = p1 - p2 > 0 ? p1 - p2 : p1 + p2;
    if (t > 0 && t < bestHit.distance)
    {
        bestHit.distance = t;
        bestHit.position = ray.origin + t * ray.direction;
        bestHit.normal = normalize(bestHit.position - sphere.position);
        //bestHit.color = sphere.color;

        bestHit.albedo = sphere.albedo;
        bestHit.specular = sphere.specular;
        bestHit.smoothness = sphere.smoothness;
        bestHit.emission = sphere.emission;
    }
}


RayHit Trace(Ray ray)
{
        RayHit bestHit = CreateRayHit();
        uint count = SphereArr.spheres.length();
        int i;

        // Trace ground plane
        IntersectPlane(ray, bestHit, groundplane);

        // Trace spheres
        for (i = 0; i < count; i++)
        {
                IntersectSphere(ray, bestHit, SphereArr.spheres[i]);
        }

        return bestHit;
}


//-------------------------------------
//- SAMPLING

mat3 GetTangentSpace(vec3 normal)
{
    // Choose a helper vector for the cross product
    vec3 helper = vec3(1, 0, 0);
    if (abs(normal.x) > 0.99f)
        helper = vec3(0, 1, 0); // switched last two

    // Generate vectors
    vec3 tangent = normalize(cross(normal, helper));
    vec3 binormal = normalize(cross(normal, tangent));
    return mat3(tangent, binormal, normal);
}

vec3 SampleHemisphere(vec3 normal, float alpha)
{
    // Sample the hemisphere, where alpha determines the kind of the sampling
    float cosTheta = pow(rand(), 1.0f / (alpha + 1.0f));
    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
    float phi = 2 * PI * rand();
    vec3 tangentSpaceDir = vec3(cos(phi) * sinTheta, cosTheta, sin(phi) * sinTheta); // switched the last two?

    // Transform direction to world space
    return GetTangentSpace(normal) * tangentSpaceDir;
}


float SmoothnessToPhongAlpha(float s)
{
    return pow(1000.0f, s * s);
}


vec3 Shade(inout Ray ray, RayHit hit)
{
    if (hit.distance < infinity )
    {
        // Calculate chances of diffuse and specular reflection
        hit.albedo = min(1.0f - hit.specular, hit.albedo);
        float specChance = energy(hit.specular);
        float diffChance = energy(hit.albedo);

        // Roulette-select the ray's path
        float roulette = rand();
        if (roulette < specChance)
        {
            // Specular reflection
            ray.origin = hit.position + hit.normal * 0.001f;
            float alpha = SmoothnessToPhongAlpha(hit.smoothness);
            ray.direction = SampleHemisphere(reflect(ray.direction, hit.normal), alpha);
            float f = (alpha + 2) / (alpha + 1);
            ray.energy *= (1.0f / specChance) * hit.specular * sdot(hit.normal, ray.direction, f);
        }
        else if (diffChance > 0 && roulette < specChance + diffChance)
        {
            // Diffuse reflection
            ray.origin = hit.position + hit.normal * 0.001f;
            ray.direction = SampleHemisphere(hit.normal, 1.0f);
            ray.energy *= (1.0f / diffChance) * hit.albedo;
        }
        else
        {
            // Terminate ray
            ray.energy = vec3(0.0f);
        }

        return hit.emission;
    }
    else
    {
        // Erase the ray's energy - the sky doesn't reflect anything
        ray.energy = vec3(0.2f);

        // Sample the skybox and write it
        return texture(skybox, vec3(-ray.direction.y, ray.direction.z, ray.direction.x)).xyz; // no need to normalize texcoord
    }
}

/*
vec3 Shade(inout Ray ray, RayHit hit)
{
    if (hit.distance < infinity) {
        //return hit.color;

        vec3 specular = vec3(0.6f, 0.6f, 0.6f);
        // Reflect the ray and multiply energy with specular reflection
        ray.origin = hit.position + hit.normal * 0.001f;
        ray.direction = reflect(ray.direction, hit.normal);
        ray.energy *= specular;
        // Return nothing
        return vec3(0.0f, 0.0f, 0.0f);
    }
    else {
        ray.energy = 0.0f;
        //float theta = acos(ray.direction.z) / -PI;
        //float phi = atan(ray.direction.y, -ray.direction.x) / -PI * 0.5f;
        //texture(skybox, vec3(phi, theta, 0));
        return texture(skybox, vec3(-ray.direction.y, ray.direction.z, ray.direction.x)).xyz; // no need to normalize texcoord
    }
}
*/

void main ()
{
    // base pixel colour for image
    vec4 pixel = vec4 (0.0, 0.0, 0.0, 1.0);
    // get index in global work group i.e x,y position
    ivec2 pixel_coords = ivec2 (gl_GlobalInvocationID.xy);

    float max_x = 5.0;
    float max_y = 5.0;

    ivec2 dims = imageSize (img_output); // fetch image dimensions

    float x = (float(pixel_coords.x * 2 - dims.x) / dims.x);
    float y = (float(pixel_coords.y * 2 - dims.y) / dims.y);

    ////
    Ray ray = CreateCameraRay(vec2(x,y));

    vec3 result = vec3(0.f, 0.f, 0.f);
    for (int i = 0; i < 8; i++)
    {
        RayHit hit = Trace(ray);
        result += ray.energy * Shade(ray, hit);
        if (ray.energy.x < 1e-6 && ray.energy.y < 1e-6 && ray.energy.z < 1e-6)
            break;
    }

    //if (hit.distance < infinity)
    //    result = hit.color;
    //else
    //    result = vec4(0.f,0.f,0.f,0.f);
    pixel = vec4(result, 1.f);

    // output to a specific pixel in the image
    imageStore (img_output, pixel_coords, pixel);
}


