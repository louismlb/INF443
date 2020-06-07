#include "shader.hpp"

#include "vcl/base/base.hpp"

#include <vector>
#include <iostream>
#include <cassert>

namespace vcl
{

static void check_compilation(GLuint shader,const std::string& shader_str)
{
    GLint is_compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);

    // get info on compilation
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> infoLog( static_cast<size_t>(maxLength)+1 );
    glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

    if( maxLength >= 1 )
    {
        std::cerr << "[Info from shader compilation]"<< std::endl;
        std::cerr << &infoLog[0] << std::endl;
        std::cerr << "For shader " << shader_str << std::endl;
    }

    if( is_compiled==GL_FALSE )
    {
        std::cerr << "Compilation Failed" <<std::endl;
        glDeleteShader(shader);
        exit(1);
    }
}

static void check_link(GLuint vertex_shader, GLuint fragment_shader, GLuint program)
{
    GLint is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);

    // Get info on Link
    GLint maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> infoLog( static_cast<size_t>(maxLength)+1 );
    glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

    if( maxLength >= 1 )
    {
        std::cerr << "[Info from shader Link]"<< std::endl;
        std::cerr << &infoLog[0] << std::endl;
    }
    if( is_linked==GL_FALSE ) //if failed link
    {
        // clean memory
        glDeleteProgram( program );
        glDeleteShader( vertex_shader);
        glDeleteShader( fragment_shader);

        std::cerr << "Failed to link shader program" << std::endl;
        exit(1);
    }

}

GLuint compile_shader(const std::string& shader_str, const GLenum shader_type)
{
    const GLuint shader = glCreateShader(shader_type); assert( glIsShader(shader) );
    char const* const shader_cstring = shader_str.c_str();
    glShaderSource(shader, 1, &shader_cstring, nullptr);

    // Compile shader
    glCompileShader( shader );

    check_compilation(shader, shader_str);

    return shader;
}


GLuint create_shader_program(const std::string& vertex_shader_path, const std::string& fragment_shader_path)
{
    const std::string vertex_shader_str   = read_file_text(vertex_shader_path);
    const std::string fragment_shader_str = read_file_text(fragment_shader_path);

    const GLuint vertex_shader   = compile_shader(vertex_shader_str, GL_VERTEX_SHADER);
    const GLuint fragment_shader = compile_shader(fragment_shader_str, GL_FRAGMENT_SHADER);

    assert( glIsShader(vertex_shader) );
    assert( glIsShader(fragment_shader) );

    // Create Program
    const GLuint program = glCreateProgram();
    assert( glIsProgram(program) );

    // Attach Shader to Program
    glAttachShader( program, vertex_shader );
    glAttachShader( program, fragment_shader );

    // Link Program
    glLinkProgram( program );

    check_link(vertex_shader, fragment_shader, program);

    // Shader can be detached.
    glDetachShader( program, vertex_shader);
    glDetachShader( program, fragment_shader);


    return program;
}

GLuint create_shader_program(const std::string& vertex_shader_path, const std::string& geometry_shader_path, const std::string& fragment_shader_path)
{
    const std::string vertex_shader_str   = read_file_text(vertex_shader_path);
    const std::string geometry_shader_str = read_file_text(geometry_shader_path);
    const std::string fragment_shader_str = read_file_text(fragment_shader_path);

    const GLuint vertex_shader   = compile_shader(vertex_shader_str, GL_VERTEX_SHADER);
    const GLuint geometry_shader = compile_shader(geometry_shader_str, GL_GEOMETRY_SHADER);
    const GLuint fragment_shader = compile_shader(fragment_shader_str, GL_FRAGMENT_SHADER);

    assert( glIsShader(vertex_shader) );
    assert( glIsShader(geometry_shader) );
    assert( glIsShader(fragment_shader) );

    // Create Program
    const GLuint program = glCreateProgram();
    assert( glIsProgram(program) );

    // Attach Shader to Program
    glAttachShader( program, vertex_shader );
    glAttachShader( program, geometry_shader );
    glAttachShader( program, fragment_shader );

    // Link Program
    glLinkProgram( program );

    check_link(vertex_shader, fragment_shader, program);

    // Shader can be detached.
    glDetachShader( program, vertex_shader);
    glDetachShader( program, geometry_shader);
    glDetachShader( program, fragment_shader);


    return program;
}

static void check_compute_link(GLuint compute_shader, GLuint program)
{
    GLint is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);

    // Get info on Link
    GLint maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> infoLog( static_cast<size_t>(maxLength)+1 );
    glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

    if( maxLength >= 1 )
    {
        std::cerr << "[Info from shader Link]"<< std::endl;
        std::cerr << &infoLog[0] << std::endl;
    }
    if( is_linked==GL_FALSE ) //if failed link
    {
        // clean memory
        glDeleteProgram( program );
        glDeleteShader( compute_shader );

        std::cerr << "Failed to link shader program" << std::endl;
        exit(1);
    }

}

GLuint create_compute_shader_program(const std::string& compute_shader_path)
{
    const std::string compute_shader_str = read_file_text( compute_shader_path );

    const GLuint compute_shader = compile_shader( compute_shader_str, GL_COMPUTE_SHADER );

    assert( glIsShader( compute_shader ) );

    // Create Program
    const GLuint compute_program = glCreateProgram();
    assert( glIsProgram( compute_program ) );

    // Attach Shader to Program
    glAttachShader( compute_program, compute_shader );

    // Link Program
    glLinkProgram( compute_program );

    // Check Link
    check_compute_link(compute_shader, compute_program);

    return compute_program;
}


void compute_query() { // query up the workgroups
        int work_grp_cnt[3];
        int work_grp_size[3];
        int work_grp_inv;
        int work_grp_mem;

        // maximum global work group (total work in a dispatch)
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

        std::cout<<"max total work items  x: "<<work_grp_cnt[0];
        std::cout<<" y: "<<work_grp_cnt[1]<<" z: "<<work_grp_cnt[2]<<std::endl;

        // maximum local work group (one shader's slice)
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

        std::cout<<"max local work groups  x: "<<work_grp_size[0];
        std::cout<<" y: "<<work_grp_size[1]<<" z: "<<work_grp_size[2]<<std::endl;

        // maximum compute shader invocations (x * y * z)
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
        std::cout<<"max local work group invocations : "<<work_grp_inv<<std::endl;

        // maximum compute shared memory, vars declared as shared float/vec/... in the shader
        glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &work_grp_mem);
        std::cout<<"max shared memory in a local group (bytes) : "<<work_grp_mem<<std::endl;
}


void ComputeShaderProgram::SetVersion(const std::string& shader_version)
{
    this->shader_str = shader_version;
}


void ComputeShaderProgram::AddShaderString(const std::string& string_path)
{
    const std::string shader_string = read_file_text( string_path );
    this->shader_str += shader_string;
}


void ComputeShaderProgram::CompileShader()
{
    //const std::string compute_shader_str = read_file_text( this->shader_path );
    // old code, before multiple strings

    this->shader = compile_shader( this->shader_str, GL_COMPUTE_SHADER );

    assert( glIsShader( this->shader ) );
}


void ComputeShaderProgram::CreateProgram()
{
    this->program = glCreateProgram();

    assert( glIsProgram( this->program ) );

    // Attach Shader to Program
    glAttachShader( this->program, this->shader );

    // Link Program
    glLinkProgram( this->program );

    // Check Link
    check_compute_link( this->shader, this->program );
}


void ComputeShaderProgram::Disable()
{
    glDetachShader(this->program, this->shader);
    glDeleteShader(this->shader);
    glDeleteProgram(this->program);
}

/* Old code structure
void ComputeShaderProgram::CreateProgram(const std::string& shader_path)
{
    this->SetVersion(shader_path);
    this->CompileShader();
    this->CreateProgram();
}
*/

void ComputeShaderProgram::CreateComplexProgram(const buffer<std::string> shader_list)
{
    this->SetVersion("#version 430\n");
    for (std::string shader_to_add : shader_list)
        this->AddShaderString(shader_to_add);
    this->CompileShader();
    this->CreateProgram();
}


void ComputeShaderProgram::MemBarrier( GLbitfield barriers ) {
    glMemoryBarrier(barriers);
}


void ComputeShaderProgram::UniformI(const GLchar * name, GLint a, GLint b, GLint c, GLint d) {
    GLint location = glGetUniformLocation(this->program, name);
    glUniform4i(location, a, b, c, d);
}
void ComputeShaderProgram::UniformI(const GLchar * name, GLint a, GLint b, GLint c) {
    GLint location = glGetUniformLocation(this->program, name);
    glUniform3i(location, a, b, c);
}
void ComputeShaderProgram::UniformI(const GLchar * name, GLint a, GLint b) {
    GLint location = glGetUniformLocation(this->program, name);
    glUniform2i(location, a, b);
}
void ComputeShaderProgram::UniformI(const GLchar * name, GLint a) {
    GLint location = glGetUniformLocation(this->program, name);
    glUniform1i(location, a);
}

void ComputeShaderProgram::UniformUI(const GLchar * name, GLuint a, GLuint b, GLuint c, GLuint d) {
    GLint location = glGetUniformLocation(this->program, name);
    glUniform4ui(location, a, b, c, d);
}
void ComputeShaderProgram::UniformUI(const GLchar * name, GLuint a, GLuint b, GLuint c) {
    GLint location = glGetUniformLocation(this->program, name);
    glUniform3ui(location, a, b, c);
}
void ComputeShaderProgram::UniformUI(const GLchar * name, GLuint a, GLuint b) {
    GLint location = glGetUniformLocation(this->program, name);
    glUniform2ui(location, a, b);
}
void ComputeShaderProgram::UniformUI(const GLchar * name, GLuint a) {
    GLint location = glGetUniformLocation(this->program, name);
    glUniform1ui(location, a);
}

void ComputeShaderProgram::UniformF(const GLchar * name, GLfloat a, GLfloat b, GLfloat c, GLfloat d) {
    GLint location = glGetUniformLocation(this->program, name);
    glUniform4f(location, a, b, c, d);
}
void ComputeShaderProgram::UniformF(const GLchar * name, GLfloat a, GLfloat b, GLfloat c) {
    GLint location = glGetUniformLocation(this->program, name);
    glUniform3f(location, a, b, c);
}
void ComputeShaderProgram::UniformF(const GLchar * name, GLfloat a, GLfloat b) {
    GLint location = glGetUniformLocation(this->program, name);
    glUniform2f(location, a, b);
}
void ComputeShaderProgram::UniformF(const GLchar * name, GLfloat a) {
    GLint location = glGetUniformLocation(this->program, name);
    glUniform1f(location, a);
}

void ComputeShaderProgram::UniformVecI(const GLchar * name, GLsizei count, int vecsize, GLint *value) {
    GLint location = glGetUniformLocation(this->program, name);
    switch (vecsize) {
        case 1:
            glUniform1iv(location, count, value);
            break;
        case 2:
            glUniform2iv(location, count, value);
            break;
        case 3:
            glUniform3iv(location, count, value);
            break;
        case 4:
            glUniform4iv(location, count, value);
            break;
        default:
            break;
    }

}

void ComputeShaderProgram::UniformVecUI(const GLchar * name, GLsizei count, int vecsize, GLuint *value) {
    GLint location = glGetUniformLocation(this->program, name);
    switch (vecsize) {
        case 1:
            glUniform1uiv(location, count, value);
            break;
        case 2:
            glUniform2uiv(location, count, value);
            break;
        case 3:
            glUniform3uiv(location, count, value);
            break;
        case 4:
            glUniform4uiv(location, count, value);
            break;
        default:
            break;
    }
}

void ComputeShaderProgram::UniformVecF(const GLchar * name, GLsizei count, int vecsize, GLfloat *value) {
    GLint location = glGetUniformLocation(this->program, name);
    switch (vecsize) {
        case 1:
            glUniform1fv(location, count, value);
            break;
        case 2:
            glUniform2fv(location, count, value);
            break;
        case 3:
            glUniform3fv(location, count, value);
            break;
        case 4:
            glUniform4fv(location, count, value);
            break;
        default:
            break;
    }
}


void ComputeShaderProgram::Run()
{
    glUseProgram(this->program);
}

void ComputeShaderProgram::Dispatch(GLuint x, GLuint y, GLuint z)
{
    glDispatchCompute(x, y, z);
}

void ComputeShaderProgram::Stop()
{
    glUseProgram(0);
}





}
