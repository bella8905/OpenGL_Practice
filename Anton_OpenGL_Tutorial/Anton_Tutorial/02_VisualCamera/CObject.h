#pragma once

#include <vector>
#include <string>
#include "Utl_Include.h"

using std::vector;
using std::string;

class CCObject
{
public:
    CCObject();
    virtual ~CCObject() = 0;

protected:
    bool _inited;
    vector<vec3> _points;
    GLuint _vao, _verVBO, _colorVBO, _normalVBO; 
    GLuint _sp; // shader program this obj is using

protected:
    bool initModel( const GLuint& t_linkedSP ) = 0;

public:
    void DrawModel();

};



class CCTriangle : public CCObject {
public:
    CCTriangle() {}
    ~CCTriangle() {}

protected:
    bool initModel( const GLuint& t_linkedSP );
};


// a object read from a model file
class CCModel : public CCObject {
public:
    CCModel( const string& t_file ) { _fileName = t_file; }
    ~CCModel() {}

protected:
    string _fileName;

protected:
    bool initModel( const GLuint& t_linkedSP );
};