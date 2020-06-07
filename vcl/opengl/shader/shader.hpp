#pragma once

#include "vcl/wrapper/glad/glad.hpp"
#include "vcl/vcl.hpp"

#include <string>

namespace vcl
{

/** Compile an individual shader provided as a string.
 * Check that the compilation succeed. */
GLuint compile_shader(const std::string& shader_str, const GLenum shader_type);


/** Compile vertex and fragment shaders provided from their file paths, link them, and return a shader program.
 * Check that link operation succeed. */
GLuint create_shader_program(const std::string& vertex_shader_path, const std::string& fragment_shader_path);

GLuint create_shader_program(const std::string& vertex_shader_path, const std::string& geometry_shader_path, const std::string& fragment_shader_path);

GLuint create_compute_shader_program(const std::string& compute_shader_path);

void compute_query();



class ComputeShaderProgram
{
public :
    GLuint shader;
    GLuint program;

    void SetVersion(const std::string& shader_version);
    void AddShaderString(const std::string& shader_path);
    void CompileShader();
    void CreateProgram();

    void Disable();

    //void CreateProgram(const std::string& shader_path);

    void CreateComplexProgram(const buffer<std::string> shader_list);


    void MemBarrier( GLbitfield barriers );

    void Run();
    void Dispatch(GLuint x, GLuint y, GLuint z);
    void Stop();

    // Have to be sent after program use started!
    void UniformI(const GLchar * name, GLint a, GLint b, GLint c, GLint d);
    void UniformI(const GLchar * name, GLint a, GLint b, GLint c);
    void UniformI(const GLchar * name, GLint a, GLint b);
    void UniformI(const GLchar * name, GLint a);

    void UniformUI(const GLchar * name, GLuint a, GLuint b, GLuint c, GLuint d);
    void UniformUI(const GLchar * name, GLuint a, GLuint b, GLuint c);
    void UniformUI(const GLchar * name, GLuint a, GLuint b);
    void UniformUI(const GLchar * name, GLuint a);

    void UniformF(const GLchar * name, GLfloat a, GLfloat b, GLfloat c, GLfloat d);
    void UniformF(const GLchar * name, GLfloat a, GLfloat b, GLfloat c);
    void UniformF(const GLchar * name, GLfloat a, GLfloat b);
    void UniformF(const GLchar * name, GLfloat a);

    void UniformVecI(const GLchar * name, GLsizei count, int vecsize, GLint *value);
    void UniformVecUI(const GLchar * name, GLsizei count, int vecsize, GLuint *value);
    void UniformVecF(const GLchar * name, GLsizei count, int vecsize, GLfloat *value);


private :
    std::string shader_str;

};


}
