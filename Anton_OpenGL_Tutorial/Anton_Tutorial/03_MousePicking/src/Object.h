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
#pragma once

#include "Utl_Include.h"
#include "Material.h"
#include "Geometry.h"

class CShader;

class CObj
{
public:
    CObj();
    virtual ~CObj();

public:
    GEO_TYPE _type;
    CMaterial _material;
    mat4 _modelMat;
    CShader* _shader;

public:
    void DrawObj();    
};

