/////////////////////////////////////////////////////////////////
//
//  Practice_11_19_14 - material
//
//  Implementation of Anton's OpenGL tutorial
//
//  Copyright (c) 2014 Bella Q
//  
/////////////////////////////////////////////////////////////////

#pragma once

#include "Utl_Include.h"

class CMaterial
{
public:
    CMaterial( const CColor& t_ks, const CColor& t_kd, const CColor& t_ka, const float& t_splExp );
    virtual ~CMaterial(void);

protected:
    CColor _ks;     // specular reflectance factor
    float _splExp;  // specular power
    CColor _kd;     // diffuse reflectance factor
    CColor _ka;     // ambient reflectance factor
};

