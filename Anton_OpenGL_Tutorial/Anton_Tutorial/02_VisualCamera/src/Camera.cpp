#include "Camera.h"


CCamera::CCamera() : _pos( vec3(0)), _face( vec3(0)), _up( vec3(0)), _near(0), _far(0), _FOV(0), _aspect(0), _viewMat( mat4(1.f) ), _projMat( mat4(1.f) ), _ready( false ) 
{
}


CCamera::~CCamera(void)
{
}


void CCamera::Setup( const vec3& t_pos, const vec3& t_face, const vec3& t_up, const float& t_near, const float& t_far, const float &t_FOV, const float& t_aspect ) {
    _pos = t_pos;
    _up = glm::normalize( t_up );
    _face = glm::normalize( t_face );
    _near = t_near;
    _far = t_far;
    _FOV = t_FOV;
    _aspect = t_aspect;

    assert( _near > 0 && _near < _far && _FOV > 0 && _FOV < 180 && t_aspect > 0 );
    _ready = true;


    calViewMat();
    calProjMat();


}

void CCamera::calViewMat() {
    if( !_ready )
    {
        LogError<<"camera not ready"<<LogEndl;
        return;
    }
    
    vec3 z = -_face ;
    vec3 x = glm::cross( _up, z );
    vec3 y = glm::cross( z, x );

    // inverse of a translation matrix
    // T(v)^-1 = T(-v)
    mat4 T( vec4( 1.f, 0.f, 0.f, 0.f),
            vec4( 0.f, 1.f, 0.f, 0.f ),
            vec4( 0.f, 0.f, 1.f, 0.f ),
            vec4( -_pos, 1.f ) );

    // inverse of a rotation matrix
    // for orthonormalized matrix,
    // T(v)^-1 = transpose( T(v) )
    mat4 R( vec4( x.x, y.x, z.x, 0.f ),
            vec4( x.y, y.y, z.y, 0.f ),
            vec4( x.z, y.z, z.z, 0.f ),
            vec4( 0.f, 0.f, 0.f, 1.f ) );

    _viewMat = R * T;
}


void CCamera::calProjMat() {
    if( !_ready )
    {
        LogError<<"camera not ready"<<LogEndl;
        return;
    }

    float range = tan( _FOV * 0.5f * Utl::g_o2Pi ) * _near;
    float sx = _near / ( range * _aspect );
    float sy = _near / range;
    float sz = -( _far + _near ) / ( _far - _near );
    float pz = -( 2.f * _far * _near ) / ( _far - _near );

    _projMat =  mat4( vec4( sx, 0.f, 0.f, 0.f ),
                      vec4( 0.f, sy, 0.f, 0.f ),
                      vec4( 0.f, 0.f, sz, -1.f ),
                      vec4( 0.f, 0.f, pz, 0.f ) );

}

void CSimpleCamera::SetPos( const vec3& t_newPos ) {
    _pos = t_newPos;
    calViewMat();
}