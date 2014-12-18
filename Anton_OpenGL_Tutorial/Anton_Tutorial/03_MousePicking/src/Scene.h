#pragma once

#include "Object.h"

class CScene
{
public:
    CScene();
    virtual ~CScene();

public:
    vector<CObj> _objects;

public:
    void DrawScene();
    void AddObj( CObj& t_obj ); 

    // get the first hit object by a ray
    int GetRayHitObjIdx( const Utl::SRay& t_ray );
};

