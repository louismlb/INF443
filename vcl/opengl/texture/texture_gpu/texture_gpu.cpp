#include "texture_gpu.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <third_party/stb/stb_image.h>

namespace vcl
{

//GLuint create_texture_gpu(const image& im)
//{
//    return create_texture_gpu(im.data, im.width, im.height);
//}

GLuint create_texture_gpu(std::vector<unsigned char> const& data, GLsizei width, GLsizei height, GLint wrap_s, GLint wrap_t)
{
    GLuint id = 0;
    glGenTextures(1,&id);
    //glActiveTexture(GL_TEXTURE8); ////// test..
    glBindTexture(GL_TEXTURE_2D,id);

    // Send texture on GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set default texture behavior
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D,0);

    return id;
}

GLuint create_texture_gpu(image_raw const& im, GLint wrap_s, GLint wrap_t)
{
    return create_texture_gpu(im.data, GLsizei(im.width), GLsizei(im.height), wrap_s, wrap_t);
}

//void update_texture_gpu(GLuint texture_id, image_rgb const& im)
//{
//    assert(glIsTexture(texture_id));

//    glBindTexture(GL_TEXTURE_2D, texture_id);
//    glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, GLsizei(im.size()[0]), GLsizei(im.size()[1]), GL_RGB, GL_UNSIGNED_BYTE, &im.data[0][0]);
//    glGenerateMipmap(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D,0);

////    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
////    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
////    draw(quad, scene.camera);
////    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
//}


GLuint create_texture_gpu(buffer2D<vec3> const& im, GLint wrap_s, GLint wrap_t)
{
    GLuint id = 0;
    glGenTextures(1,&id);
    glBindTexture(GL_TEXTURE_2D,id);

    // Send texture on GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, GLsizei(im.dimension[0]), GLsizei(im.dimension[1]), 0, GL_RGB, GL_FLOAT, &im.data[0][0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set default texture behavior
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D,0);

    return id;
}
void update_texture_gpu(GLuint texture_id, buffer2D<vec3> const& im)
{
    assert_vcl(glIsTexture(texture_id), "Incorrect texture id");

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, GLsizei(im.dimension[0]), GLsizei(im.dimension[1]), GL_RGB, GL_FLOAT, &im.data[0][0]);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);
}


GLuint load_skybox_texture(const std::vector<std::string>& faces)
{

    GLuint textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            std::cout<< "Cubemap face "<< faces[i] << " loaded." << std::endl;
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    std::cout<< "Cubemap texture ID : " << textureID <<std::endl;
    return textureID;
}


GLuint compute_texture(int tex_w, int tex_h, GLenum texture)
{
    GLuint textureID;

    glGenTextures( 1, &textureID );

    glActiveTexture( texture );
    glBindTexture( GL_TEXTURE_2D, textureID );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    // linear allows to scale the window up retaining reasonable quality
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    // same internal format as compute shader input
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, nullptr );
    // bind to image unit so can write to specific pixels from the shader
    glBindImageTexture( 0, textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F );

    std::cout<< "Compute texture ID " << textureID << " bound to " << texture <<std::endl;
    return textureID;
}



}
