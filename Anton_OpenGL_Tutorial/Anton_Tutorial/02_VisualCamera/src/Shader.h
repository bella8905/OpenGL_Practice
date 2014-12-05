#pragma once

#include <string>
#include "Camera.h"

class CShader
{
public:
    // CShader( const string& t_vs, const string& t_fs );
    CShader( const std::string& t_vs, const std::string& t_fs, const std::string& t_gs = "", const std::string& t_ts = "" );
    virtual ~CShader();

protected:
    std::string _vs, _fs, _ts, _gs;
    GLuint _sp;
    bool _inited;

protected:
    virtual void initSP() = 0;
    // create a shader obj, return the shader obj index
    std::string glTypeToString( GLenum t_type );
    void printShaderInfoLog( const GLuint& t_shaderIndex );
    void printSPInfoLog( const GLuint& t_spIndex) ;
    bool checkShaderCompileStatus( const GLuint& t_shaderIndex );
    bool checkSPLinkingStatus( const GLuint& t_spIndex );
    bool validateSP( const GLuint& t_spIndex );
    void printSPInfo( const GLuint& sp);
    GLint createShader( const char* t_shaderFile, GLenum t_shaderType );
    GLuint createShaderProgram( const GLint& t_vs, const GLint& t_gs, const GLint& t_ts, const GLint& t_fs );
    void createShaderProgram();
    bool readFileToStr( const char* t_file, std::string& t_fileContentStr  );

public:
    // attrib location
    GLuint _attr_pos;

public:
    virtual void BindShader();
};


// a simple shader which creates a perspective view
class CPerspCamShader : public CShader {
public:
    CPerspCamShader( CCamera* t_cam );
    ~CPerspCamShader() {}

public:
    // uniform variable names
    GLint _uni_inputColorLoc, _uni_viewMatLoc, _uni_projMatLoc;

    vec4 _vertexColor;

private:
    static const std::string _VS_FILE;
    static const std::string _FS_FILE;

    CCamera* _camera;

protected:
    virtual void initSP();

public:
    virtual void BindShader();
};