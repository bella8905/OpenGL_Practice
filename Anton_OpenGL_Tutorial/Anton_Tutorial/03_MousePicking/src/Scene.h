#pragma once

#include "Object.h"

class CScene
{
public:
    CScene();
    virtual ~CScene();

private:
    vector<CObj> _objects;

public:
    void DrawScene();
    void AddObj( CObj& t_obj ); 
};

