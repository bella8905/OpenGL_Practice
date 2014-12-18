#include <algorithm>
#include "Object.h"


CObj::CObj( GEO_TYPE t_type ) : _geoType( t_type ), _material( g_defaultMat ), _modelMat( mat4() ), _invModelMat( mat4() ), _shaderType( SD_SINGLE_COLOR ), _drawBB( false )
{
}


CObj::~CObj()
{
}


void CObj::DrawObj() {
    CGeoContainer::GetInstance().DrawGeo( _geoType, _shaderType, &_material, _modelMat, _drawBB );
}

float CObj::rayIntersectWithBB( const Utl::SRay& t_ray, const SBoundBox* t_bb ) {
    Utl::SRay ProjRay = t_ray.Transform( _invModelMat );
    vec4 posObj =  ProjRay._Origin;
    vec4 dirObj = ProjRay._Dir;

    // if xd, yd, zd equals to zero, skip this direction
    float tNear = -std::numeric_limits<float>::infinity();
    float tFar = std::numeric_limits<float>::infinity();
    float t1, t2;

    if( Utl::Equals( dirObj[0], 0.f ) && Utl::Equals( dirObj[1], 0.f ) && Utl::Equals( dirObj[2], 0.f ) )   return -1.f;

    for( int i = 0 ; i < 3; ++i )
    {
        if( Utl::Equals( dirObj[i], 0.f) )
        {
            if( posObj[i] > t_bb->_max[i] || posObj[i] < t_bb->_min[i] )  return -1.f;
        }
        else
        {
            t1 = ( t_bb->_min[i] - posObj[i] ) / dirObj[i];   
            t2 = ( t_bb->_max[i] - posObj[i] ) / dirObj[i];
            tNear = max( tNear, min( t1, t2 ) );
            tFar = min( tFar, max( t1, t2 ) );

            if( tNear > tFar || tFar < 0 )  return -1.f;
        }
    }

    vec4 IntersectPoint = ProjRay.GetPointOnRay( tNear );
    IntersectPoint = _modelMat * IntersectPoint;
    // test where the intersect point resides
    vec4 Normal = vec4( 0.f );
    for( us i = 0; i < 3; ++i )
    {
        if( Utl::Equals( IntersectPoint[ i ], t_bb->_min[ i ] ) ) 
        {
            Normal[ i ] = -1.f;
            break;
        }
        else if( Utl::Equals( IntersectPoint[ i ], t_bb->_max[ i ] ) )
        {
            Normal[ i ] = 1.f; 
            break;
        }
    }

    // Assert( Equals ( glm::length( Normal ), 1.f ), "normal not assigned correctly" );
    Normal = glm::normalize( _modelMat * Normal );

    return glm::length( IntersectPoint - t_ray._Origin );;
}

float CObj::RayIntersectTest( const Utl::SRay& t_ray ) {
    SBoundBox* bb = CGeoContainer::GetInstance().GetGeoBB( _geoType );
    assert( bb );
    return rayIntersectWithBB( t_ray, bb );
}