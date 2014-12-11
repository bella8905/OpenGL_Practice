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
    CLight( const vec3& t_pos, const Utl::CColor& t_ls, const Utl::CColor& t_ld, const Utl::CColor& t_la );
    virtual ~CLight(void);

protected:
    vec3 _pos;
    Utl::CColor _ls;     // specular light color
    Utl::CColor _ld;     // diffuse light color
    Utl::CColor _la;     // ambient light color
};

