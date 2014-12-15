/////////////////////////////////////////////////////////////////
//
//  Practice_11_19_14 - object
//
//  Implementation of Anton's OpenGL tutorial
//
// an obj is the real object in scene
// which includes the geo type, model transformation, material info, and shader info
//
//  Copyright (c) 2014 Bella Q
//  
/////////////////////////////////////////////////////////////////
#include "Utl_Include.h"
#include "Material.h"

class CShader;

#pragma once
class CObj
{
public:
    CObj(void);
    virtual ~CObj(void);

protected:
    mat4 _modelMat;
    CMaterial _material;
    CShader* _shader;

public:
    void SetShader( CShader )

};

