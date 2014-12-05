#pragma once

#include <vector>
#include <string>
#include "Utl_Include.h"
#include "Shader.h"

using std::vector;
using std::string;

class CObject
{
public:
    CObject( CShader* t_shader );
    virtual ~CObject() = 0;

protected:
    bool _inited;
    GLuint _vao, _verVBO, _colorVBO, _normalVBO; 
    CShader* _shader;

protected:
    virtual void initModel() = 0;

public:
    void DrawModel();

};



class CTriangle : public CObject {
public:
    CTriangle( CShader* t_shader  ) : CObject( t_shader ) { initModel(); }
    ~CTriangle() {}

protected:
    void initModel();
};


// a object read from a model file
class CModel : public CObject {
public:
    CModel( CShader* t_shader, const string& t_file ) : CObject( t_shader ), _fileName( t_file ) { initModel(); }
    ~CModel() {}

protected:
    string _fileName;

protected:
    void initModel();
};