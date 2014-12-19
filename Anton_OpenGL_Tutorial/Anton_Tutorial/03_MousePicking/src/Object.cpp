#include <algorithm>
#include "Object.h"

bool CObj::_drawBB = true;
bool CObj::_arcball_drawAcball = true;
float CObj::_arcball_radius = 0.8f;

CObj::CObj( GEO_TYPE t_type ) : _geoType( t_type ), _material( g_defaultMat ), _modelMat( mat4() ), _invModelMat( mat4() ), _shaderType( SD_SINGLE_COLOR ), _selected( false )
{
}


CObj::~CObj()
{
}

void CObj::SetModelMat( const mat4& t_modelMat ) {
    _modelMat = t_modelMat; 
    _invModelMat = glm::inverse( _modelMat ); 
    // set arcball center
    SBoundBox* sphereGeoBB = CGeoContainer::GetInstance().GetGeoBB( GEO_UNIT_SPHERE );
    assert( sphereGeoBB );
    _arcball._center = vec3( t_modelMat * vec4( sphereGeoBB->_center, 1.f ) );
}

void CObj::DrawObj() {
    CGeoContainer::GetInstance().DrawGeo( _geoType, _shaderType, &_material, _modelMat, _selected && _drawBB );

    if( _selected &&_arcball_drawAcball  ) {
        // draw an arcball
        mat4 scaleMat = mat4(   vec4( _arcball_radius, 0.f,            0.f,            0.f ), 
            vec4( 0.f,            _arcball_radius, 0.f,            0.f ), 
            vec4( 0.f,            0.f,            _arcball_radius, 0.f ), 
            vec4( 0.f,            0.f,            0.f,            1.f ) );

        mat4 translateMat = mat4(   vec4( 1.f, 0.f, 0.f, 0.f ),
            vec4( 0.f, 1.f, 0.f, 0.f ), 
            vec4( 0.f, 0.f, 1.f, 0.f ), 
            vec4( _arcball._center, 1.f ) );

        mat4 arcballMat = translateMat * scaleMat;
        CGeoContainer::GetInstance().DrawGeo( GEO_UNIT_SPHERE, SD_PHONG, &g_defaultMat, arcballMat, false );

        // if it is during rotation, draw start and end points
        if( _arcball._isInRot ) {
            // draw start and end point

        }
    }
}


// ray box intersect
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

float CObj::RayIntersectTestWithBB( const Utl::SRay& t_ray ) {
    SBoundBox* bb = CGeoContainer::GetInstance().GetGeoBB( _geoType );
    assert( bb );
    return rayIntersectWithBB( t_ray, bb );
}


// ray sphere intersect
void CObj::RayIntersectTestWithArcball( const Utl::SRay& t_ray, const bool& t_isStart ) {
    Utl::SRay ProjRay = t_ray.Transform( _invModelMat );
    vec4 posObj =  ProjRay._Origin;
    vec4 dirObj = ProjRay._Dir;

    // solve function: At2 + Bt + C = 0
    float A = dirObj.x * dirObj.x + dirObj.y * dirObj.y + dirObj.z * dirObj.z;
    float B = 2 * ( ( posObj.x - _arcball._center.x ) * dirObj.x + ( posObj.y - _arcball._center.y ) * dirObj.y + ( posObj.z - _arcball._center.z ) * dirObj.z );
    float C = ( posObj.x - _arcball._center.x ) * ( posObj.x - _arcball._center.x ) + ( posObj.y - _arcball._center.y ) * ( posObj.y - _arcball._center.y ) + ( posObj.z - _arcball._center.z ) * ( posObj.z - _arcball._center.z ) - _arcball_radius * _arcball_radius;

    if( Utl::Equals( A, 0.f ) )
    {
        LogError<< "anything wrong with ray?" <<LogEndl;
        return;
    }

    float Radicand = B * B - 4 * A * C;
    if( Radicand < 0 )  return;

    float t1 = ( -B + sqrt( Radicand ) ) * 0.5f / A;
    float t2 = ( -B - sqrt( Radicand ) ) * 0.5f / A;

    float tmax = max( t1, t2 );
    float tmin = min( t1, t2 );

    if( tmax < 0 ) return;

    float t;
    if( tmin >= 0 ) t = tmin; 
    else t = tmax;

    vec4 InstPoint =  ProjRay.GetPointOnRay( t );
    InstPoint = _modelMat * InstPoint;
    if( t_isStart ) {
        _arcball._rotStartPoint = vec3( InstPoint );
        StartRot();
    }
    else {
        _arcball._rotEndPoint = vec3( InstPoint );
    }
}