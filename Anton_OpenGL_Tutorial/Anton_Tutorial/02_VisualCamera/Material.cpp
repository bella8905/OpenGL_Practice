/////////////////////////////////////////////////////////////////
//
//  Practice_11_19_14 - material
//
//  Implementation of Anton's OpenGL tutorial
//
//  Copyright (c) 2014 Bella Q
//  
/////////////////////////////////////////////////////////////////

#include "Material.h"


CMaterial::CMaterial( const Utl::CColor& t_ks, const Utl::CColor& t_kd, const Utl::CColor& t_ka, const float& t_splExp ) : _ks(t_ks), _kd(t_kd), _ka(t_ka), _splExp(t_splExp)
{
}


CMaterial::~CMaterial(void)
{
}