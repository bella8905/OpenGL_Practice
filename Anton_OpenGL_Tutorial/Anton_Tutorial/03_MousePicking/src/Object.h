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

struct SArcball {
    vec3 _center;   // center of the obj, caled by center of bb and model matrix
    bool _isInRot;
    vec3 _rotStartPoint;
    vec3 _rotEndPoint;

    SArcball() : _center( 0.f ), _isInRot( false ) {
    }

};

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

    bool _selected;
    static bool _drawBB;
    static bool _arcball_drawAcball;
    static float _arcball_radius;

protected:
    mat4 _modelMat;
    mat4 _invModelMat;

    // arcball
    SArcball _arcball;
    float rayIntersectWithBB( const Utl::SRay& t_ray, const SBoundBox* t_bb );
    

public:
    void DrawObj();  
    void SetModelMat( const mat4& t_modelMat );
    
    // test if bb is hit by ray, and return the dist from ray origin to hit point
    float RayIntersectTestWithBB( const Utl::SRay& t_ray );
    void RayIntersectTestWithArcball( const Utl::SRay& t_ray, const bool& t_isStart );

    void StartRot() { _arcball._isInRot = true; }
    void EndRot() { _arcball._isInRot = false; }
};

