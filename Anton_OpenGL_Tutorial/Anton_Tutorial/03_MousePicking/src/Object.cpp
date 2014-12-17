#include "Object.h"


CObj::CObj( GEO_TYPE t_type ) : _geoType( t_type ), _material( g_defaultMat ), _modelMat( mat4() ), _shaderType( SD_SINGLE_COLOR ), _drawBB( false )
{
}


CObj::~CObj()
{
}


void CObj::DrawObj() {
    CGeoContainer::GetInstance().DrawGeo( _geoType, _shaderType, &_material, _modelMat, _drawBB );
}
