/////////////////////////////////////////////////////////////////
//
//  Practice_11_19_14 - shader
//
//  Implementation of Anton's OpenGL tutorial
//
//  Copyright (c) 2014 Bella Q
//  
/////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include "Camera.h"

class CObject;

class CShader
{
public:
    // CShader( const string& t_vs, const string& t_fs );
    CShader() : _inited( false ), _sp(0) {}
    virtual ~CShader() {}

protected:
    std::string _vs, _fs, _ts, _gs;
    GLuint _sp;
    bool _inited;

protected:
    virtual void initSP( const std::string& t_vs, const std::string& t_fs, const std::string& t_gs = "", const std::string& t_ts = "" );
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
    void BindShader();
};


// a simple shader which creates a perspective view
class CPerspCamShader : public CShader {
public:
    CPerspCamShader( CCamera* t_cam );
    ~CPerspCamShader() {}

public:
    // uniform variable names
    GLint _uni_inputColorLoc, _uni_viewMatLoc, _uni_projMatLoc, _uni_modelMatLoc;

    vec4 _vertexColor;

private:
    CCamera* _camera;

protected:
    virtual void initSP( const std::string& t_vs, const std::string& t_fs, const std::string& t_gs = "", const std::string& t_ts = "" );

public:
    virtual void BindShaderWithObject( CObject* t_object );
};


class CPhongShader : public CPerspCamShader {
public:
    CPhongShader( CCamera* t_cam );
    ~CPhongShader() {} 

protected:
    virtual void initSP( const std::string& t_vs, const std::string& t_fs, const std::string& t_gs = "", const std::string& t_ts = "" );

};
