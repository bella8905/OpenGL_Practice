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
    GLuint _vao;
    CShader* _shader;
    glm::mat4 _modelMat;

protected:
    virtual bool initModel() = 0;

public:
    virtual void DrawModel() = 0;
    bool IsInited() { return _inited; }

};



class CTriangle : public CObject {
public:
    CTriangle( CShader* t_shader  ) : CObject( t_shader ) { initModel(); }
    ~CTriangle() {}

protected:
    bool initModel();

public:
    virtual void DrawModel();
};


// a object read from a model file
class CModel : public CObject {
public:
    CModel( CShader* t_shader, const string& t_file ) : CObject( t_shader ), _fileName( t_file ) { initModel(); }
    ~CModel() {}

protected:
    string _fileName;
    int _numOfVertices;

protected:
    bool initModel();

public:
    virtual void DrawModel();
};