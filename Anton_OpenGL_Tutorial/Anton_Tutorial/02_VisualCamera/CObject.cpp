#include "CObject.h"


CCObject::CCObject() : _inited( false ), _vao( 0 ), _verVBO( 0 ), _colorVBO( 0 ), _normalVBO( 0 ), _sp( 0 ) {
    initModel();
}


CCObject::~CCObject(void)
{
}

void CCObject::DrawModel() {
    if( !_inited ) {
        LogError<<"model not inited"<<LogEndl;
        return;
    }


}


// triangle
bool CCTriangle::initModel( const GLuint& t_linkedSP ) {
    _points.clear();
    _points.push_back( vec3( 0.0f, 0.5f, 0.0f ) );
    _points.push_back( vec3( 0.5f, -0.5f, 0.0f ) );
    _points.push_back( vec3( -0.5f, -0.5f, 0.0f ) );

    // generate vao and vbos
    glGenBuffers( 1, &_verVBO );
    glBindBuffer( GL_ARRAY_BUFFER, _verVBO );
    glBufferData( GL_ARRAY_BUFFER, _points.size(), &_points[0], GL_STATIC_DRAW );

    glGenVertexArrays( 1, &_vao );
    glBindVertexArray( _vao );
    glBindBuffer( GL_ARRAY_BUFFER, _verVBO );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 0 );

    _sp = t_linkedSP;

    _inited = true;
}


// model
bool CCModel::initModel() {
    // read from model file
}