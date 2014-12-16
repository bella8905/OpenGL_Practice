#include "Object.h"


CObj::CObj() : _type( GEO_TRIANGLE ), _material( g_defaultMat ), _modelMat( mat4() ), _shader( 0 )
{
}


CObj::~CObj()
{
}


void CObj::DrawObj() {
    
}