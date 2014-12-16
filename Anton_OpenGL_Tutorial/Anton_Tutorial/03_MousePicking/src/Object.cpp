#include "Object.h"


CObj::CObj( GEO_TYPE t_type ) : _type( t_type ), _material( g_defaultMat ), _modelMat( mat4() ), _shader( 0 )
{
}


CObj::~CObj()
{
}


void CObj::DrawObj() {
    CGeoContainer::GetInstance().DrawGeo( _type, _shader, &_material, _modelMat );
}
