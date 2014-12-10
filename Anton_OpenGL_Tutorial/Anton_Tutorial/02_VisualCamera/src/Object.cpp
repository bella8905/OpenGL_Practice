/////////////////////////////////////////////////////////////////
//
//  Practice_11_19_14 - object
//
//  Implementation of Anton's OpenGL tutorial
//
//  Copyright (c) 2014 Bella Q
//  
/////////////////////////////////////////////////////////////////

#include "Object.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"


CObject::CObject( CShader* t_shader ) : _inited( false ), _shader( t_shader ), _scale( 1.0f ), _modelMat( 1.f ) {
}


CObject::~CObject(void)
{
}


// void CObject::calModelMat() {
//     mat4 scaleMat = mat4( vec4( _scale, 0.f,      0.f,     0.f ), 
//                           vec4( 0.f,    _scale,   0.f,     0.f ), 
//                           vec4( 0.f,    0.f,     _scale,   0.f ), 
//                           vec4( 0.f,    0.f,     0.f,      1.f ) );
//     mat4 rotMat = mat4( vec4( _rot[0], 0.f ), 
//                         vec4( _rot[1], 0.f ), 
//                         vec4( _rot[2], 0.f ), 
//                         vec4( 0.f, 0.f, 0.f, 1.f ) );
//     mat4 translateMat = mat4( vec4( 1.f, 0.f, 0.f, 0.f ),
//                               vec4( 0.f, 1.f, 0.f, 0.f ), 
//                               vec4( 0.f, 0.f, 1.f, 0.f ), 
//                               vec4( _translate, 1.f ) );
// 
//     _modelMat = translateMat * rotMat * scaleMat;
// }

void CObject::SetScales( const float& t_scales ) {
    float scale = t_scales / _scale;
    mat4 scaleMat = mat4(   vec4( scale, 0.f,      0.f,     0.f ), 
                            vec4( 0.f,    scale,   0.f,     0.f ), 
                            vec4( 0.f,    0.f,     scale,   0.f ), 
                            vec4( 0.f,    0.f,     0.f,      1.f ) );

    _modelMat = scaleMat * _modelMat;
    _scale = t_scales;
}

// triangle
bool CTriangle::initModel() {
    assert( _shader != 0 );

    if( _inited )   return true;

    vector<vec3> points;
    points.push_back( vec3( 0.0f, 0.5f, 0.0f ) );
    points.push_back( vec3( 0.5f, -0.5f, 0.0f ) );
    points.push_back( vec3( -0.5f, -0.5f, 0.0f ) );

    // generate vao and vbos
    glGenBuffers( 1, &_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo );
    glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( float ), &points[0], GL_STATIC_DRAW );

    glGenVertexArrays( 1, &_vao );
    glBindVertexArray( _vao );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo );

    glVertexAttribPointer( _shader->_attr_pos, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 0 );

    _inited = true;

    return _inited;
}

void CTriangle::deinitModel() {
    if( !_inited )  return;
    glDeleteBuffers( 1, &_vbo );
}

void CTriangle::DrawModel() {
    if( !_inited ) {
        LogError<<"model not inited"<<LogEndl;
        return;
    }

    CPerspCamShader* pPerspCamShader = dynamic_cast<CPerspCamShader*>( _shader );
    if( pPerspCamShader ) {
        pPerspCamShader->BindShaderWithObject( this );
    }
    else {
        _shader->BindShader();
    }

    glBindVertexArray( _vao );
    glDrawArrays( GL_TRIANGLES, 0, 3 );

}

// model
void CModel::SMesh::InitMesh( const aiMesh* t_aiMesh, bool t_unified ) {
    if( _inited )   return;
    assert( t_aiMesh );

    int numOfVertices = t_aiMesh->mNumVertices;
    LogMsg<<"   vertices: "<< numOfVertices<<LogEndl;

    _hasTex = t_aiMesh->HasTextureCoords(0);
    _hasFaces = t_aiMesh->HasFaces();


    glGenVertexArrays( 1, &_vao );
    glBindVertexArray( _vao );

    // vbo
/*    if( t_aiMesh.HasPositions() ) {*/
        vector<GLfloat> points;
        for( int i = 0; i < numOfVertices; ++i )
        {
            const aiVector3D* point = &( t_aiMesh->mVertices[ i ] );
            points.push_back( ( GLfloat )point->x );
            points.push_back( ( GLfloat )point->y );
            points.push_back( ( GLfloat )point->z );

            if( t_unified ) {
                if( i == 0 ) {
                    _bounds._min = vec3( point->x, point->y, point->z );
                    _bounds._max = vec3( point->x, point->y, point->z );
                }
                else {
                    _bounds.SetBounds( vec3( point->x, point->y, point->z ) );
                }
            }
        }

        glGenBuffers( 1, &_vbo );
        glBindBuffer( GL_ARRAY_BUFFER, _vbo );
        glBufferData( GL_ARRAY_BUFFER, 3 * numOfVertices * sizeof( GLfloat ), &points[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 0 );

/*    }*/

    // nbo
/*    if( t_aiMesh.HasNormals() ) {*/
        vector<GLfloat> normals;
        for( int i = 0; i < numOfVertices; ++i )
        {
            const aiVector3D* normal = &( t_aiMesh->mNormals[ i ] );
            normals.push_back( ( GLfloat )normal->x );
            normals.push_back( ( GLfloat )normal->y );
            normals.push_back( ( GLfloat )normal->z );
        }

        glGenBuffers( 1, &_nbo );
        glBindBuffer( GL_ARRAY_BUFFER, _nbo );
        glBufferData( GL_ARRAY_BUFFER, 3 * numOfVertices * sizeof( GLfloat ), &normals[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 1 );
/*    }*/

    // tbo
    // a vertex can have a number of textures bound. 
    // we are only using the first one
    if( _hasTex ) {
        vector<GLfloat> texcoords;
        for( int i = 0; i < numOfVertices; ++i )
        {
            const aiVector3D* point = &( t_aiMesh->mTextureCoords[ 0 ][ i ] );
            texcoords.push_back( ( GLfloat )point->x );
            texcoords.push_back( ( GLfloat )point->y );
        }

        glGenBuffers( 1, &_tbo );
        glBindBuffer( GL_ARRAY_BUFFER, _tbo );
        glBufferData( GL_ARRAY_BUFFER, 2 * numOfVertices * sizeof( GLfloat ), &texcoords[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 2 );
    }


    // ibo
    if( _hasFaces ) {
        vector<GLuint> indices;
        for( unsigned int i = 0; i < t_aiMesh->mNumFaces; ++i ) {
            const aiFace* face = &( t_aiMesh->mFaces[ i ] );
            for( unsigned int j = 0; j < face->mNumIndices; ++j ) {
                indices.push_back( face->mIndices[ j ] );
            }
        }

        _numOfIndices = indices.size();
        glGenBuffers( 1, &_ibo );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, _numOfIndices * sizeof( GLuint ), &indices[0], GL_STATIC_DRAW );
    }

    _inited = true;

}

void CModel::SMesh::DeinitMesh() {
    if( !_inited )  return;

/*    if( t_aiMesh->HasPositions() ) {*/
        glDeleteBuffers( 1, &_vbo );
/*    }*/
/*    if( t_aiMesh->HasNormals() ) {*/
        glDeleteBuffers( 1, &_nbo );
/*    }*/

    if( _hasTex ) {
        glDeleteBuffers( 1, &_tbo );
    }

    if( _hasFaces ) {
        glDeleteBuffers( 1, &_ibo );
    }

    _inited = false;
}

void CModel::SMesh::DrawMesh() {
    if( !_inited ) return;

    glBindVertexArray( _vao );
    glDrawElements( GL_TRIANGLES, _numOfIndices, GL_UNSIGNED_INT, NULL );
}

bool CModel::initModel() {
    if( _inited )   return true;
    // read from model file
    // do it in a c++ way
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( _fileName, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals );

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


    // mesh
    _meshes.clear();
    
    SBoundBox bounds; 
    for( unsigned int i = 0; i < scene->mNumMeshes; ++i ) {
        const aiMesh* assMesh = scene->mMeshes[ i ];
        SMesh mesh;
        mesh.InitMesh( assMesh, _unified );
        _meshes.push_back( mesh );

        if( _unified ) {
           if( i == 0 ) {
               bounds = mesh._bounds;
           }
           else {
               bounds.SetBounds( mesh._bounds );
           }
        }
    }

    if( _unified ) {
        _adjustedTranslate = -bounds.GetCenter();
        _adjustedScale = 2.f / bounds.GetLongestSide();

        mat4 scaleMat = mat4(   vec4( _adjustedScale, 0.f,            0.f,            0.f ), 
                                vec4( 0.f,            _adjustedScale, 0.f,            0.f ), 
                                vec4( 0.f,            0.f,            _adjustedScale, 0.f ), 
                                vec4( 0.f,            0.f,            0.f,            1.f ) );

        mat4 translateMat = mat4(   vec4( 1.f, 0.f, 0.f, 0.f ),
                                    vec4( 0.f, 1.f, 0.f, 0.f ), 
                                    vec4( 0.f, 0.f, 1.f, 0.f ), 
                                    vec4( _adjustedTranslate, 1.f ) );

        // translate the model first to center and then scale
        _modelMat = _modelMat * scaleMat * translateMat;
    }
 
    _inited = true;
    return _inited;
}

void CModel::deinitModel() {
    if( !_inited ) return;
    for( unsigned int i = 0; i < _meshes.size(); ++i ) {
        _meshes[i].DeinitMesh();
    }

    _inited = false;
}

void CModel::DrawModel() {
    if( !_inited ) {
        LogError<<"model not inited"<<LogEndl;
        return;
    }

    CPerspCamShader* pPerspCamShader = dynamic_cast<CPerspCamShader*>( _shader );
    if( pPerspCamShader ) {
        pPerspCamShader->BindShaderWithObject( this );
    }
    else {
        _shader->BindShader();
    }

    for( unsigned int i = 0; i < _meshes.size(); ++i ) {
        _meshes[ i ].DrawMesh();
    }
}
