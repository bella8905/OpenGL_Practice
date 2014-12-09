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

    vector<vec3> points;
    points.push_back( vec3( 0.0f, 0.5f, 0.0f ) );
    points.push_back( vec3( 0.5f, -0.5f, 0.0f ) );
    points.push_back( vec3( -0.5f, -0.5f, 0.0f ) );

    GLuint vbo;
    // generate vao and vbos
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( float ), &points[0], GL_STATIC_DRAW );

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
    const aiScene* scene = importer.ReadFile( _fileName, aiProcess_Triangulate/* | aiProcess_JoinIdenticalVertices*/ );

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
    int numOfVertices = mesh->mNumVertices;
    LogMsg<<"   "<< numOfVertices <<" vertices in mesh["<<meshIndex<<"]"<<LogEndl;


    glGenVertexArrays( 1, &_vao );
    glBindVertexArray( _vao );


    if( mesh->HasPositions() ) {
        vector<GLfloat> points;
        for( int i = 0; i < numOfVertices; ++i )
        {
            const aiVector3D* point = &( mesh->mVertices[ i ] );
            points.push_back( ( GLfloat )point->x );
            points.push_back( ( GLfloat )point->y );
            points.push_back( ( GLfloat )point->z );
        }
        

        GLuint vbo;
        glGenBuffers( 1, &vbo );
        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glBufferData( GL_ARRAY_BUFFER, 3 * numOfVertices * sizeof( GLfloat ), &points[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 0 );

    }
    if( mesh->HasNormals() ) {
        vector<GLfloat> normals;
        for( int i = 0; i < numOfVertices; ++i )
        {
            const aiVector3D* normal = &( mesh->mNormals[ i ] );
            normals.push_back( ( GLfloat )normal->x );
            normals.push_back( ( GLfloat )normal->y );
            normals.push_back( ( GLfloat )normal->z );
        }


        GLuint vbo;
        glGenBuffers( 1, &vbo );
        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glBufferData( GL_ARRAY_BUFFER, 3 * numOfVertices * sizeof( GLfloat ), &normals[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 1 );
    }

    // a vertex can have a number of textures bound. 
    // we are only using the first one
    if( mesh->HasTextureCoords( 0 ) ) {
        vector<GLfloat> texcoords;
        for( int i = 0; i < numOfVertices; ++i )
        {
            const aiVector3D* point = &( mesh->mTextureCoords[ 0 ][ i ] );
            texcoords.push_back( ( GLfloat )point->x );
            texcoords.push_back( ( GLfloat )point->y );
        }


        GLuint vbo;
        glGenBuffers( 1, &vbo );
        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glBufferData( GL_ARRAY_BUFFER, 2 * numOfVertices * sizeof( GLfloat ), &texcoords[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 2 );
    }

    if( mesh->HasFaces() ) {
        vector<GLuint> indices;
        for( unsigned int i = 0; i < mesh->mNumFaces; ++i ) {
            const aiFace* face = &( mesh->mFaces[ i ] );
            for( unsigned int j = 0; j < face->mNumIndices; ++j ) {
                indices.push_back( face->mIndices[ j ] );
            }
        }

        _numOfIndices = indices.size();
        GLuint ibo;
        glGenBuffers( 1, &ibo );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, _numOfIndices * sizeof( GLuint ), &indices[0], GL_STATIC_DRAW );
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
    glDrawElements( GL_TRIANGLES, _numOfIndices, GL_UNSIGNED_INT, NULL );
}
