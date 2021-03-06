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

class CGeo;
class CLight;
class CMaterial;

class CShader
{
public:
    // CShader( const string& t_vs, const string& t_fs );
    CShader() : _spCreated( false ), _sp(0) {}
    virtual ~CShader() {}

protected:
    std::string _vs, _fs, _ts, _gs;
    GLuint _sp;
    bool _spCreated;

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
    /*GLuint _attr_pos;*/
    // always set attribute loc
    // 1: pos
    // 2: normal
    // 3: texcoords

public:
    void BindShader();
    virtual void BindShaderWithObjectForDrawing( CGeo* t_object, CMaterial* t_material, const mat4& t_trandform ) = 0;
};


// a simple shader which creates a perspective view
class CPerspCamShader : public CShader {
public:
    CPerspCamShader();
    ~CPerspCamShader() = 0;

public:
    // uniform variable names
    GLint _uni_inputColorLoc, _uni_viewMatLoc, _uni_projMatLoc, _uni_modelMatLoc;

    vec4 _vertexColor;

protected:
    CCamera* _camera;

protected:
    virtual void initSP( const std::string& t_vs, const std::string& t_fs, const std::string& t_gs = "", const std::string& t_ts = "" );

public:
    void BindShaderWithObjectForDrawing( CGeo* t_object, CMaterial* t_material, const mat4& t_trandform  );
    void SetCamera( CCamera* t_camera ) { _camera = t_camera; }
};

class CSingleColorShader : public CPerspCamShader {
public:
    CSingleColorShader();
    ~CSingleColorShader() {} 
};


class CPhongShader : public CPerspCamShader {
public:
    CPhongShader();
    ~CPhongShader() {} 

protected:
    // uniform location
    GLint _uni_lightPos, _uni_lightLs, _uni_lightLd, _uni_lightLa, _uni_mtlKs, _uni_mtlKd, _uni_mtlKa, _uni_mtlSplExp;
    CLight* _light;

protected:
    virtual void initSP( const std::string& t_vs, const std::string& t_fs, const std::string& t_gs = "", const std::string& t_ts = "" );

public:
    void BindShaderWithObjectForDrawing( CGeo* t_object, CMaterial* t_material, const mat4& t_transform  );
    void SetLight( CLight* t_light  ) { _light = t_light; }
};



// a shader used to draw object bound box
class CTestNormalShader: public CPerspCamShader {
public:
    CTestNormalShader();
    ~CTestNormalShader() {} 
};



// shader container
// a interface to get all possible shaders we have 
enum SHADER_TYPE { SD_SINGLE_COLOR, SD_PHONG, SD_NORMAL_TEST ,SD_COUNTER };
class CShaderContainer {
private:
    CShaderContainer();
    CShaderContainer( const CShaderContainer& t_cont );
    void operator=( const CShaderContainer& t_cont );

private:
    CShader* _shaders[ SD_COUNTER ];
    bool _inited;

public:
    static CShaderContainer& GetInstance() {
        static CShaderContainer instance;
        return instance;
    }

    void Init();
    void Deinit();

    void BindShaderForDrawing( SHADER_TYPE t_type, CGeo* t_object, CMaterial* t_material, const mat4& t_transform );
};
