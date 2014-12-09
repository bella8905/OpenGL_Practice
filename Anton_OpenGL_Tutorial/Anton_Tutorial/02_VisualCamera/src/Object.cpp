#include "Object.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"


CObject::CObject( CShader* t_shader ) : _inited( false ), _vao( 0 ), _shader( t_shader ), _modelMat( mat4( 1.f ) ) {
    // initModel();
}


CObject::~CObject(void)
{
}




// triangle
bool CTriangle::initModel() {
    assert( _shader != 0 );

    _inited = false;

    GLfloat points[] = {
        0.0f, 0.5f, 0.0f, 
        0.5f, -0.5f, 0.0f, 
        -0.5f, -0.5f, 0.0f
    };

    GLuint vbo;
    // generate vao and vbos
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( points), points, GL_STATIC_DRAW );

    glGenVertexArrays( 1, &_vao );
    glBindVertexArray( _vao );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );

    glVertexAttribPointer( _shader->_attr_pos, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 0 );


    _inited = true;

    return _inited;
}


void CTriangle::DrawModel() {
    if( !_inited ) {
        LogError<<"model not inited"<<LogEndl;
        return;
    }

    _shader->BindShader();
    glBindVertexArray( _vao );
    glDrawArrays( GL_TRIANGLES, 0, 3 );

}

// model
bool CModel::initModel() {
    _inited = false;
    // read from model file
    // do it in a c++ way
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( _fileName, aiProcess_Triangulate );

    if( !scene ) {
        LogError<<"can't read model: "<< _fileName <<LogEndl;
        return false;
    }

    // scene processing
    LogMsg<<"***************Model loaded: "<<_fileName<<LogEndl;
    LogMsg<<"   "<<scene->mNumAnimations<<" animations"<<LogEndl;
    LogMsg<<"   "<<scene->mNumCameras<<" cameras"<<LogEndl;
    LogMsg<<"   "<<scene->mNumLights<<" lights"<<LogEndl;
    LogMsg<<"   "<<scene->mNumMaterials<<" materials"<<LogEndl;
    LogMsg<<"   "<<scene->mNumMeshes<<" meshes"<<LogEndl;
    LogMsg<<"   "<<scene->mNumTextures<<" textures"<<LogEndl;

    // onlyl process first mesh for now
    // keep track of the first vertex index for each mesh for drawing
    int meshIndex = 0;
    const aiMesh* mesh = scene->mMeshes[meshIndex];
    _numOfVertices = mesh->mNumVertices;
    LogMsg<<"   "<< _numOfVertices <<" vertices in mesh["<<meshIndex<<"]"<<LogEndl;

    GLfloat* points = 0;
    GLfloat* normals = 0;
    GLfloat* texcoords = 0;

    glGenVertexArrays( 1, &_vao );
    glBindVertexArray( _vao );


    if( mesh->HasPositions() ) {
        points = new GLfloat[ _numOfVertices * 3 ];
        for( int i = 0; i < _numOfVertices; ++i )
        {
            const aiVector3D* point = &( mesh->mVertices[ i ] );
            points[ i * 3 ] = ( GLfloat )point->x;
            points[ i * 3 + 1 ] = ( GLfloat )point->y;
            points[ i * 3 + 2 ] = ( GLfloat )point->z;
        }
        

        GLuint vbo;
        glGenBuffers( 1, &vbo );
        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glBufferData( GL_ARRAY_BUFFER, 3 * _numOfVertices * sizeof( GLfloat ), points, GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 0 );

        delete[] points;
        points = 0;
    }
    if( mesh->HasNormals() ) {
        normals = new GLfloat[ _numOfVertices * 3 ];
        for( int i = 0; i < _numOfVertices; ++i )
        {
            const aiVector3D* normal = &( mesh->mNormals[ i ] );
            normals[ i * 3 ] = ( GLfloat )normal->x;
            normals[ i * 3 + 1 ] = ( GLfloat )normal->y;
            normals[ i * 3 + 2 ] = ( GLfloat )normal->z;
        }


        GLuint vbo;
        glGenBuffers( 1, &vbo );
        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glBufferData( GL_ARRAY_BUFFER, 3 * _numOfVertices * sizeof( GLfloat ), normals, GL_STATIC_DRAW );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 1 );

        delete[] normals;
        normals = 0;
    }

    // a vertex can have a number of textures bound. 
    // we are only using the first one
    if( mesh->HasTextureCoords( 0 ) ) {
        texcoords = new GLfloat[ _numOfVertices * 2 ];
        for( int i = 0; i < _numOfVertices; ++i )
        {
            const aiVector3D* point = &( mesh->mTextureCoords[ 0 ][ i ] );
            texcoords[ i * 2 ] = ( GLfloat )point->x;
            texcoords[ i * 2 + 1 ] = ( GLfloat )point->y;
        }


        GLuint vbo;
        glGenBuffers( 1, &vbo );
        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glBufferData( GL_ARRAY_BUFFER, 3 * _numOfVertices * sizeof( GLfloat ), texcoords, GL_STATIC_DRAW );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 2 );

        delete[] texcoords;
        texcoords = 0;
    }

    _inited = true;
    return _inited;
}

void CModel::DrawModel() {
    if( !_inited ) {
        LogError<<"model not inited"<<LogEndl;
        return;
    }

    _shader->BindShader();
    glBindVertexArray( _vao );
    glDrawArrays( GL_TRIANGLES, 0, _numOfVertices );
}
