#include "Object.h"


CObj::CObj( GEO_TYPE t_type ) : _geoType( t_type ), _material( g_defaultMat ), _modelMat( mat4() ), _shaderType( SD_SINGLE_COLOR )
{
}


CObj::~CObj()
{
}


void CObj::DrawObj( bool t_drawBB ) {
    CGeoContainer::GetInstance().DrawGeo( _geoType, _shaderType, &_material, _modelMat, t_drawBB );
}
