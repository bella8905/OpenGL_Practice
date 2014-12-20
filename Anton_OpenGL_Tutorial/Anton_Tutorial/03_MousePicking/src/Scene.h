#pragma once

#include "Object.h"

class CScene
{
public:
    CScene();
    virtual ~CScene();

public:
    vector<CObj> _objects;
    int _selectedObjIdx;

public:
    void DrawScene();
    void AddObj( CObj& t_obj ); 
    void UpdateScene();
    void StopRotObj(const int& t_objIdx );
    // get the first hit object by a ray
    int GetRayHitObjIdx( const Utl::SRay& t_ray );
    // update arcball
    void RayIntersectWithArcball( const Utl::SRay& t_ray, const bool& t_isStart );
};

