#version 430 core

in struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
} fragment;

uniform sampler2D texture_sampler;

out vec4 FragColor;

uniform vec3 camera_position;
uniform vec3 color = vec3(1.0, 1.0, 1.0);
uniform float color_alpha = 1.0;
uniform float ambiant  = 0.2;
uniform float diffuse  = 0.8;
uniform float specular = 0.5;
uniform int specular_exponent = 128;

//vec3 light = vec3(camera_position.x, camera_position.y, camera_position.z); // moving light source
vec3 light = vec3(0,0,100); // for fixed light source

vec4 colorInterp(float z, float z1, float z2, vec4 c1, vec4 c2) {
    float t = (z - z1)/(z2-z1);
    return (1-t) * c1 + t * c2;
}

float z_offset = -1.5;

float z_white = 4.2 + z_offset;
float z_brown = 3.5 + z_offset;
float z_lbrown = 2.4 + z_offset;
float z_green = 1.8 + z_offset;
float z_lgreen = .3 + z_offset;
float z_sand = .1 + z_offset;

vec4 c_white = vec4(.9, .92, 1, 1);
vec4 c_brown = vec4(.48, .35, .3, 1);
vec4 c_lbrown = vec4(.74, .5, .35, 1);
vec4 c_green = vec4(.35, .55, .3, 1);
vec4 c_lgreen = vec4(.45, .7, .35, 1);
vec4 c_sand = vec4(.89, .82, .5, 1);

vec4 colorFromHeight(float z) {
    vec4 newcolor;
    /*
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
    */

    //blanc
    if (z_white < z) {
        newcolor = c_white;
    }
    //marron foncé -> blanc
    if (z_brown < z && z < z_white) {
        newcolor = colorInterp(z, z_brown, z_white, c_brown, c_white);
    }
    //marron clair -> foncé
    if (z_lbrown < z && z < z_brown) {
        newcolor = colorInterp(z, z_lbrown, z_brown, c_lbrown, c_brown);
    }
    //vert foncé -> marron clair
    if (z_green < z && z < z_lbrown) {
        newcolor = colorInterp(z, z_green, z_lbrown, c_green, c_lbrown);
    }
    //vert clair -> foncé
    if (z_lgreen < z && z < z_green) {
        newcolor = colorInterp(z, z_lgreen, z_green, c_lgreen, c_green);
    }
    //sable -> vert clair
    if (z_sand < z && z < z_lgreen) {
        newcolor = colorInterp(z, z_sand, z_lgreen, c_sand, c_lgreen);
    }
    //sable
    if (z < z_sand) {
        newcolor = c_sand;
    }

    return newcolor;

    //return vec4(0,1,0,1);
}

void main()
{
    vec3 n = normalize(fragment.normal.xyz);
    vec3 u = normalize(light-fragment.position.xyz);
    vec3 r = reflect(u,n);
    vec3 t = normalize(fragment.position.xyz-camera_position);


    float diffuse_value  = diffuse * clamp( dot(u,n), 0.0, 1.0);
    float specular_value = specular * pow( clamp( dot(r,t), 0.0, 1.0), specular_exponent);

    vec3 white = vec3(1.0);
    vec4 color_texture = texture(texture_sampler, fragment.texture_uv);

    vec4 newcolor = colorFromHeight(fragment.position.z);

    vec3 c = (ambiant+diffuse_value) * newcolor.rgb * fragment.color.rgb * color_texture.rgb + specular_value * white; // color.rgb then fragment.color.rgb

    FragColor = vec4(c, color_texture.a*fragment.color.a*color_alpha);

}
