#pragma once

#include "vcl/wrapper/glad/glad.hpp"

#include <string>

namespace vcl
{





class ComputeBuffer
{
public :
    GLuint buffer;
    GLenum target;

    // void * data;
    int itemsize;
    int numitems;

    void SetParams(int numitems, int itemsize);

    void SetTarget(GLenum target);

    void Generate();

    void Delete();

    void Bind();

    void BindBase(GLuint index);

    void Unbind();

    void SetData(void * data, GLenum usage);

    void GetData(void * data);


private :

};





}
