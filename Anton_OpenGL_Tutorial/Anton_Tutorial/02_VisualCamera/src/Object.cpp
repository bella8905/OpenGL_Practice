#include "Object.h"


CObject::CObject( CShader* t_shader ) : _inited( false ), _vao( 0 ), _verVBO( 0 ), _colorVBO( 0 ), _normalVBO( 0 ), _shader( t_shader ) {
    initModel();
}


CObject::~CObject(void)
{
}

void CObject::DrawModel() {
    if( !_inited ) {
        LogError<<"model not inited"<<LogEndl;
        return;
    }

    _shader->BindShader();
    glBindVertexArray( _vao );
    glDrawArrays( GL_TRIANGLES, 0, 3 );

}


// triangle
void CTriangle::initModel() {
    assert( _shader != 0 );

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

    glVertexAttribPointer( _shader->_attr_pos, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 0 );


    _inited = true;
}


// model
void CModel::initModel() {
    // read from model file
}