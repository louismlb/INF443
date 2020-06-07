#include "buffer.hpp"

#include "vcl/base/base.hpp"

#include <vector>
#include <iostream>
#include <cassert>

namespace vcl
{

/*
glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, offsets_ssbo);

glBindBuffer
glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec2)*offsets.size(), &offsets[0], GL_DYNAMIC_COPY);
glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

glBindBuffer(GL_SHADER_STORAGE_BUFFER, map_ssbo);
glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float)*return_map.size(), &return_map[0]);
*/

void ComputeBuffer::Generate() {
    glGenBuffers(1, &this->buffer);
}

void ComputeBuffer::Delete() {
    glDeleteBuffers(1, &this->buffer);
}

void ComputeBuffer::Bind() {
    glBindBuffer(this->target, this->buffer);
}

void ComputeBuffer::Unbind() {
    glBindBuffer(this->target, 0);
}

void ComputeBuffer::BindBase(GLuint index) {
    glBindBufferBase(this->target, index, this->buffer);
}

void ComputeBuffer::SetData(void * data, const GLenum usage) {
    glBufferData(this->target, this->itemsize * this->numitems, data, usage);
}

void ComputeBuffer::GetData(void * data) {
    glGetBufferSubData(this->target, 0/* offset */, this->itemsize * this->numitems, data);
}

void ComputeBuffer::SetParams(int numitems, int itemsize) {
    this->itemsize = itemsize;
    this->numitems = numitems;
}

void ComputeBuffer::SetTarget(GLenum target) {
    this->target = target;
}







}
