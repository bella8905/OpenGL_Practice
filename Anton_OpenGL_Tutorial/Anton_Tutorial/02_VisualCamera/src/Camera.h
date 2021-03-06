/////////////////////////////////////////////////////////////////
//
//  Practice_11_19_14 - camera
//
//  Implementation of Anton's OpenGL tutorial
//
//  Copyright (c) 2014 Bella Q
//  
/////////////////////////////////////////////////////////////////

#pragma once
#include "Utl_Include.h"


class CCamera
{
public:
    CCamera();
    virtual ~CCamera() = 0;

protected:
    // view
    vec3 _pos, _face, _up;
    // proj, clip frustrum
    float _near, _far, _FOV, _aspect;
    mat4 _viewMat, _projMat;

    bool _ready;

public:
    enum CamType { SIMPLE = 0, NUM_TYPES, };

public:
    void Setup( const vec3& t_pos, const vec3& t_face, const vec3& t_up, const float& t_near, const float& t_far, const float &t_FOV, const float& t_aspect );

    mat4& GetViewMat() { return _viewMat; }
    mat4& GetProjMat() { return _projMat; }

    vec3& GetPos() { return _pos; }

protected:
    void calViewMat();
    void calProjMat();
};


// a simple camera can move in world space 
class CSimpleCamera : public CCamera {
public:
    CSimpleCamera() {}
    ~CSimpleCamera() {}

public:
    void SetPos( const vec3& t_newPos );
};



class CFPSCamera : public CCamera {
public:
    CFPSCamera() {}
    ~CFPSCamera() {}
};
