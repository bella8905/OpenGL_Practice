/////////////////////////////////////////////////////////////////
//
//  Practice_11_19_14 - light
//
//  Implementation of Anton's OpenGL tutorial
//
//  Copyright (c) 2014 Bella Q
//  
/////////////////////////////////////////////////////////////////

#pragma once

#include "Utl_Include.h"


class CLight
{
public:
    CLight( const vec3& t_pos, const CColor& t_ls, const CColor& t_ld, const CColor& t_la );
    virtual ~CLight(void);

protected:
    vec3 _pos;
    CColor _ls;     // specular light color
    CColor _ld;     // diffuse light color
    CColor _la;     // ambient light color
};

