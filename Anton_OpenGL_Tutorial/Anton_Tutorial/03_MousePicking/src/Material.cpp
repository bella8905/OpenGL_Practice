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


CMaterial::CMaterial( const Utl::CColor& t_kd, const bool& t_hasSpecular, const Utl::CColor& t_ks,  const float& t_splExp,  const Utl::CColor& t_ka ) : _ks(t_ks), _kd(t_kd), _ka(t_ka), _splExp(t_splExp), _hasSpecular( t_hasSpecular )
{
}


CMaterial::~CMaterial(void)
{
}


CMaterial g_defaultMat( Utl::CColor( 1.f, 0.5f, 1.f ), false, Utl::CColor(), 0.f, Utl::CColor( 1.f, 1.f, 1.f ) );