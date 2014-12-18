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
#include "Shader.h"

class CObj
{
public:
    CObj( GEO_TYPE t_type );
    virtual ~CObj();

public:
    GEO_TYPE _geoType;
    CMaterial _material;
    // CShader* _shader;
    SHADER_TYPE _shaderType;

    bool _drawBB;

protected:
    mat4 _modelMat;
    mat4 _invModelMat;

    float rayIntersectWithBB( const Utl::SRay& t_ray, const SBoundBox* t_bb );

public:
    void DrawObj();  
    void SetModelMat( const mat4& t_modelMat ) { _modelMat = t_modelMat; _invModelMat = glm::inverse( _modelMat ); }
    
    // test if bb is hit by ray, and return the dist from ray origin to hit point
    float RayIntersectTest( const Utl::SRay& t_ray );
};

