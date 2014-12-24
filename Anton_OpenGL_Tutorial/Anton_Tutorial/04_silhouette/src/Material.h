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
    CMaterial( const Utl::CColor& t_kd, const bool& t_hasSpecular, const Utl::CColor& t_ks,  const float& t_splExp,  const Utl::CColor& t_ka );
    virtual ~CMaterial();

protected:
    bool _hasSpecular;
    Utl::CColor _ks;     // specular reflectance factor
    float _splExp;  // specular power
    Utl::CColor _kd;     // diffuse reflectance factor
    Utl::CColor _ka;     // ambient reflectance factor

public:
    Utl::CColor& GetKd() { return _kd; }
    void SetKd(const Utl::CColor& t_val) { _kd = t_val; }
    Utl::CColor& GetKs() { return _ks; }
    void SetKs(const Utl::CColor& t_val) { _ks = t_val; }
    Utl::CColor& GetKa() { return _ka; }
    void SetKa(const Utl::CColor& t_val) { _ka = t_val; }
    float& GetSplExp() { return _splExp; }
    void SetSplExp(const float& t_val) { _splExp = t_val; }
    bool& GetHasSpecular() { return _hasSpecular; }
};



// a default material
extern CMaterial g_defaultMat;