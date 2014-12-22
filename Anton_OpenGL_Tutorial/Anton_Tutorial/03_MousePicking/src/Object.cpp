#include <algorithm>
#include "Object.h"

bool CObj::_drawBB = true;
bool CObj::_drawAcball = true;

float SArcball::_radius = 0.8f;
GLuint SArcball::_vao = 0;
GLuint SArcball::_vbo = 0;
GLuint SArcball::_ibo = 0;
bool SArcball::_arcball_inited = false;


void SArcball::InitArcball() {
    if( _arcball_inited ) {
        LogError<<"arcball already inited"<<LogEndl;
        return;
    }

    // we don't care the original position of the vertices
    GLfloat cubeVertices[] = {
        0.f, 0.f, 0.f,
        0.f, 0.f, 0.f,
        0.f, 0.f, 0.f,
    };

    GLushort cubeIndices [] = {
        0, 1, 2,
    };

    glGenVertexArrays( 1, &_vao );
    glBindVertexArray( _vao  );


    glGenBuffers( 1, &_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( cubeVertices ), cubeVertices, GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindVertexArray( 0 );

    glGenBuffers( 1, &_ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( cubeIndices ), cubeIndices, GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    _arcball_inited = true;
}

void SArcball::DeinitArcball() {
    if( !_arcball_inited ) {
        LogError<<"arcball not inited"<<LogEndl;
        return;
    }


    glDeleteBuffers( 1, &_vbo );
    glDeleteBuffers( 1, &_ibo );

    _arcball_inited = false;
}


// ray sphere intersect
bool SArcball::RayIntersectTestWithArcball( const Utl::SRay& t_ray, const bool& t_isStart, glm::mat3& t_rotMat ) {

    // no need to do any transformation. we use the center position and radius of arcball
    Utl::SRay ProjRay = t_ray;
    vec4 posObj =  ProjRay._Origin;
    vec4 dirObj = ProjRay._Dir;

    // solve function: At2 + Bt + C = 0
    float A = dirObj.x * dirObj.x + dirObj.y * dirObj.y + dirObj.z * dirObj.z;
    float B = 2 * ( ( posObj.x - _center.x ) * dirObj.x + ( posObj.y - _center.y ) * dirObj.y + ( posObj.z - _center.z ) * dirObj.z );
    float C = ( posObj.x - _center.x ) * ( posObj.x - _center.x ) + ( posObj.y - _center.y ) * ( posObj.y - _center.y ) + ( posObj.z - _center.z ) * ( posObj.z - _center.z ) - _radius * _radius;

    if( Utl::Equals( A, 0.f ) )
    {
        LogError<< "anything wrong with ray?" <<LogEndl;
        return false;
    }

    float Radicand = B * B - 4 * A * C;
    if( Radicand < 0 )  return false;

    float t1 = ( -B + sqrt( Radicand ) ) * 0.5f / A;
    float t2 = ( -B - sqrt( Radicand ) ) * 0.5f / A;

    float tmax = max( t1, t2 );
    float tmin = min( t1, t2 );

    if( tmax < 0 ) return false;

    float t;
    if( tmin >= 0 ) t = tmin; 
    else t = tmax;

    bool rtn;
    vec3 InstPoint =  vec3( ProjRay.GetPointOnRay( t ) );

    us vertexIndex = 0;
    if( t_isStart ) {
        _rotStartPoint = InstPoint;
        _rotEndPoint = _rotStartPoint;
        _isInRot = true;
        rtn = false;
    }
    else {
        _rotEndPoint = InstPoint;
        rtn = true;

        // calculate rot matrix
        vec3 startSide =  glm::normalize( _rotStartPoint - _center );
        vec3 endSide =  glm::normalize( _rotEndPoint - _center );
        vec3 rotAxis = glm::cross( startSide, endSide );
        float angle = acos( glm::dot( startSide, endSide ) );
        glm::quat rotQuat = glm::angleAxis( angle, rotAxis );
        t_rotMat = glm::toMat3( rotQuat );

    }

    // update our buffer data here instead of the drawing process
    // data doesn't need to be updated if we don't draw arcball
    if( CObj::_drawAcball )
    {
        vec3 vertices[] = {
            _center,
            _rotStartPoint,
            _rotEndPoint,
        };

        us numOfVertices = sizeof( vertices);
        glBindBuffer( GL_ARRAY_BUFFER, _vbo );
        glBufferSubData( GL_ARRAY_BUFFER, 0, numOfVertices, glm::value_ptr( vertices[0]  ) );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }

    return rtn;
}

void SArcball::DrawArcball() {
    // arcball model matrix
    mat4 scaleMat = mat4(   vec4( _radius, 0.f,            0.f,            0.f ), 
                            vec4( 0.f,            _radius, 0.f,            0.f ), 
                            vec4( 0.f,            0.f,            _radius, 0.f ), 
                            vec4( 0.f,            0.f,            0.f,            1.f ) );

    mat4 translateMat = mat4(   vec4( 1.f, 0.f, 0.f, 0.f ),
        vec4( 0.f, 1.f, 0.f, 0.f ), 
        vec4( 0.f, 0.f, 1.f, 0.f ), 
        vec4( _center, 1.f ) );

    mat4 modelMat = translateMat * scaleMat;

    CGeoContainer::GetInstance().DrawGeo( GEO_UNIT_SPHERE, SD_PHONG, &g_defaultMat, modelMat, false );
    if( _isInRot ) {
        // draw arcball points
        glBindVertexArray( _vao );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
        CShaderContainer::GetInstance().BindShaderForDrawing( SD_SINGLE_COLOR, 0, 0, mat4( 1.f ) );
        glDrawElements( GL_POINTS, 3, GL_UNSIGNED_SHORT, NULL  );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
    }
}

CObj::CObj( GEO_TYPE t_type ) : _geoType( t_type ), _material( g_defaultMat ), _modelMat( mat4() ), _invModelMat( mat4() ), _translate( 0.f ), _rot( 1.f ), _scale( 1.f ), _shaderType( SD_SINGLE_COLOR ), _selected( false )
{
}


CObj::~CObj()
{
}

// 
// void CObj::SetModelMat( const mat4& t_modelMat ) {
//     _modelMat = t_modelMat; 
//     _invModelMat = glm::inverse( _modelMat ); 
//     // set arcball center
//     SBoundBox* sphereGeoBB = CGeoContainer::GetInstance().GetGeoBB( GEO_UNIT_SPHERE );
//     assert( sphereGeoBB );
//     _arcball._center = vec3( t_modelMat * vec4( sphereGeoBB->_center, 1.f ) );
// }

void CObj::resetModelMatrix() {
    mat4 scaleMat = mat4(   vec4( _scale, 0.f,    0.f,    0.f ), 
        vec4( 0.f,    _scale, 0.f,    0.f ), 
        vec4( 0.f,    0.f,    _scale, 0.f ), 
        vec4( 0.f,    0.f,    0.f,    1.f ) );

    mat4 translateMat = mat4(   vec4( 1.f, 0.f, 0.f, 0.f ),
        vec4( 0.f, 1.f, 0.f, 0.f ), 
        vec4( 0.f, 0.f, 1.f, 0.f ), 
        vec4( _translate, 1.f ) );

    mat4 rotMat = mat4( vec4( _rot[0], 0.f ),
        vec4( _rot[1], 0.f ), 
        vec4( _rot[2], 0.f ),
        vec4( 0.f, 0.f, 0.f, 1.f ) );

    _modelMat = translateMat * rotMat * scaleMat;

    // not use glm::inverse to cal inverse matrix, that's probably expensive
    mat4 inv_scaleMat = mat4(   vec4( 1.f / _scale, 0.f,    0.f,    0.f ), 
        vec4( 0.f,    1.f / _scale, 0.f,    0.f ), 
        vec4( 0.f,    0.f,    1.f / _scale, 0.f ), 
        vec4( 0.f,    0.f,    0.f,    1.f ) );

    mat4 inv_translateMat = mat4(   vec4( 1.f, 0.f, 0.f, 0.f ),
        vec4( 0.f, 1.f, 0.f, 0.f ), 
        vec4( 0.f, 0.f, 1.f, 0.f ), 
        vec4( -_translate, 1.f ) );

    mat4 inv_rotMat = glm::transpose( rotMat );

    _invModelMat = inv_scaleMat * inv_rotMat * inv_translateMat;
}

void CObj::SetupModelMatrix( const vec3& t_translate, const glm::mat3 t_rot, const float& t_scale ) {
    _translate = t_translate;
    _rot = t_rot;
    _scale = t_scale;

    resetModelMatrix();

    // set arcball center to the center of object
    _arcball._center = _translate;
}

void CObj::DrawObj() {
    CGeoContainer::GetInstance().DrawGeo( _geoType, _shaderType, &_material, _modelMat, _selected && _drawBB );

    if( _selected &&_drawAcball  ) {
        _arcball.DrawArcball();
    }
}

void CObj::RotateAroundLocalAxis( const glm::mat3& t_rot ) {
    _rot = t_rot * _rot;
    resetModelMatrix();
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




void CObj::RayIntersectTestWithArcball( const Utl::SRay& t_ray, const bool& t_isStart ) {
    if( !_selected ) return;
    glm::mat3 rotMat;
    if( _arcball.RayIntersectTestWithArcball( t_ray, t_isStart, rotMat ) )
    {
        // rotate along model axis
        float theta = 30 * Utl::g_o2Pi;
        float sintheta = sin( theta );
        float costheta = cos( theta );
        glm::mat3 rotMatx(   vec3( 1.f, 0.f, 0.f ), 
                        vec3( 0.f, costheta, sintheta ),
                        vec3( 0.f, -sintheta, costheta ) );

        float phi = 10 * Utl::g_o2Pi;
        float sinphi = sin( phi );
        float cosphi = cos( phi );
        glm::mat3 rotMaty(   vec3( cosphi, 0.f, -sinphi ), 
            vec3( 0.f, 1.f, 0.f ),
            vec3( sinphi, 0.f, cosphi ) );

        rotMat = rotMatx * rotMaty;

        RotateAroundLocalAxis( rotMat );
    }
}
