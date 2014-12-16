/////////////////////////////////////////////////////////////////
//
//  Practice_11_19_14 - geometry
//
//  Implementation of Anton's OpenGL tutorial
//
// a geo class including vao, vbos and ibo for rendering
// it doesn't include any transformation,  material / shader information
// make it a singleton, and never instantiate more than 1 instances
// pass in a transformation matrix, shader and matertial for rendering
//
//  Copyright (c) 2014 Bella Q
//  
/////////////////////////////////////////////////////////////////

#include "Geometry.h"
#include "Shader.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"


CGeo::CGeo() : _inited( false ), _preprocessModelMatrix( 1.f ), _drawBoundBox( false ) {
}


CGeo::~CGeo(void)
{
}



bool CGeo::initModel() {
    return _inited;
}

void CGeo::DrawModel( CShader* t_shader, CMaterial* t_material, const mat4& t_modelMatrix ) {
    if( !_inited ) {
        LogError<<"model not inited"<<LogEndl;
        return;
    }

    assert( t_shader );
    t_shader->BindShaderWithObjectForDrawing( this, t_material, t_modelMatrix );
}

// primitive
bool CPrimGeo::initModel() {
    return CGeo::initModel();
}

void CPrimGeo::deinitModel() {
    if( !_inited )  return;
    glDeleteBuffers( 1, &_vbo );
    glDeleteBuffers( 1, &_ibo );
}

void CPrimGeo::DrawModel( CShader* t_shader, CMaterial* t_material, const mat4& t_modelMatrix) {

    CGeo::DrawModel( t_shader, t_material, t_modelMatrix );

    glBindVertexArray( _vao );
    glDrawElements( GL_TRIANGLES, _numOfIndices, GL_UNSIGNED_INT, NULL );

}

void CPrimGeo::genBufferData( const vector<SVertex>& t_vertices, const vector<GLuint>& t_indices ) {

    // generate vao and vbos
    glGenBuffers( 1, &_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( SVertex ) * t_vertices.size(), &t_vertices[0], GL_STATIC_DRAW );

    glGenVertexArrays( 1, &_vao );
    glBindVertexArray( _vao );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( SVertex ), (void*)offsetof( SVertex, _pos ) );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( SVertex ), (void*)offsetof( SVertex, _normal ) );
    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );

    glGenBuffers( 1, &_ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, _numOfIndices * sizeof( GLuint ), &t_indices[0], GL_STATIC_DRAW );

}

// triangle
bool CTriangleGeo::initModel() {
    LogMsg<<"Init Triangle"<<LogEndl;
    // if obj is already inited, simply return
    if( CPrimGeo::initModel() ) return true;

    vector<SVertex> vertices;
    vertices.push_back( SVertex( vec3( 0.0f, 0.5f, 0.0f ), vec3( 0.f, 0.f, 1.f ) ) );
    vertices.push_back( SVertex( vec3( -0.5f, -0.5f, 0.0f ), vec3( 0.f, 0.f, 1.f ) ) );
    vertices.push_back( SVertex( vec3( 0.5f, -0.5f, 0.0f ), vec3( 0.f, 0.f, 1.f ) ) );

    vector<GLuint> indices;
    indices.push_back( 0 );
    indices.push_back( 1 );
    indices.push_back( 2 );

    _numOfIndices = indices.size();
    genBufferData( vertices, indices );

    _inited = true;

    return _inited;
}

// cube
bool CCubeGeo::initModel() {
    LogMsg<<"Init Cube"<<LogEndl;
    // if obj is already inited, simply return
    if( CPrimGeo::initModel() ) return true;

    // cube ///////////////////////////////////////////////////////////////////////
    //    v6----- v5
    //   /|      /|
    //  v1------v0|
    //  | |     | |
    //  | |v7---|-|v4
    //  |/      |/
    //  v2------v3

    float cubeVertices[] = {
        0.5,0.5,0.5,  -0.5,0.5,0.5,  -0.5,-0.5,0.5,  0.5,-0.5,0.5,      // v0-v1-v2-v3
        0.5,0.5,0.5,  0.5,-0.5,0.5,  0.5,-0.5,-0.5,  0.5,0.5,-0.5,      // v0-v3-v4-v5
        0.5,0.5,0.5,  0.5,0.5,-0.5,  -0.5,0.5,-0.5,  -0.5,0.5,0.5,      // v0-v5-v6-v1
        -0.5,0.5,0.5,  -0.5,0.5,-0.5,  -0.5,-0.5,-0.5,  -0.5,-0.5,0.5,		// v1-v6-v7-v2
        -0.5,-0.5,-0.5,  0.5,-0.5,-0.5,  0.5,-0.5,0.5,  -0.5,-0.5,0.5,		// v7-v4-v3-v2
        0.5,-0.5,-0.5,  -0.5,-0.5,-0.5,  -0.5,0.5,-0.5,  0.5,0.5,-0.5 };		// v4-v7-v6-v5

    float cubeNormals[] = {
        0,0,1,  0,0,1,  0,0,1,  0,0,1,             // v0-v1-v2-v3
        1,0,0,  1,0,0,  1,0,0, 1,0,0,              // v0-v3-v4-v5
        0,1,0,  0,1,0,  0,1,0, 0,1,0,              // v0-v5-v6-v1
        -1,0,0,  -1,0,0, -1,0,0,  -1,0,0,          // v1-v6-v7-v2
        0,-1,0,  0,-1,0,  0,-1,0,  0,-1,0,        // v7-v4-v3-v2
        0,0,-1,  0,0,-1,  0,0,-1,  0,0,-1 };        // v4-v7-v6-v5


    int numOfVertices = sizeof( cubeVertices ) / sizeof( float ) / 3;

    vector<SVertex> vertices;
    for( int i = 0; i < numOfVertices; ++ i ) {
        int startIndex = 3 * i;
        vertices.push_back( SVertex( vec3( cubeVertices[ startIndex ], cubeVertices[ startIndex + 1 ], cubeVertices[ startIndex + 2 ] ), 
                                     vec3( cubeNormals[ startIndex ], cubeNormals[ startIndex + 1 ], cubeNormals[ startIndex + 2 ] ) ) );
    }

    vector<GLuint> indices;
    GLuint cubeIndices [] = {
        0,1,2,  0,2,3,  4,5,6,  4,6,7,
        8,9,10, 8,10,11, 12,13,14, 12,14,15,
        16,17,18, 16,18,19,	20,21,22, 20,22,23
    };

    _numOfIndices = sizeof( cubeIndices ) / sizeof( GLuint );
    for( int i = 0; i < _numOfIndices; ++i ) {
        indices.push_back( cubeIndices[i] );
    }

    genBufferData( vertices, indices );

    _inited = true;

    return _inited;
} 

// model
void CModelGeo::SMesh::InitMesh( const aiMesh* t_aiMesh, bool t_unified ) {
    if( _inited )   return;
    assert( t_aiMesh );

    int numOfVertices = t_aiMesh->mNumVertices;
    LogMsg<<"   vertices: "<< numOfVertices<<LogEndl;

    _hasTex = t_aiMesh->HasTextureCoords(0);
    _hasFaces = t_aiMesh->HasFaces();
    assert( _hasFaces );  // weird enough if we don't have faces in this model


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
        glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 2 );
    }


    // ibo
/*    if( _hasFaces ) {*/
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
/*    }*/

    _inited = true;

}

void CModelGeo::SMesh::DeinitMesh() {
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

/*    if( _hasFaces ) {*/
        glDeleteBuffers( 1, &_ibo );
/*    }*/

    _inited = false;
}

void CModelGeo::SMesh::DrawMesh() {
    if( !_inited ) return;

    glBindVertexArray( _vao );
    glDrawElements( GL_TRIANGLES, _numOfIndices, GL_UNSIGNED_INT, NULL );
}

CModelGeo::~CModelGeo() {
    deinitModel(); 
}

bool CModelGeo::initModel() {
    // if obj is already inited, simply return
    if( CGeo::initModel() ) return true;

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
        _preprocessModelMatrix = _preprocessModelMatrix * scaleMat * translateMat;
    }
 
    _inited = true;
    return _inited;
}

void CModelGeo::deinitModel() {
    if( !_inited ) return;
    for( unsigned int i = 0; i < _meshes.size(); ++i ) {
        _meshes[i].DeinitMesh();
    }

    _inited = false;
}

void CModelGeo::DrawModel( CShader* t_shader, CMaterial* t_material, const mat4& t_modelMatrix ) {

    CGeo::DrawModel( t_shader, t_material, t_modelMatrix );

    for( unsigned int i = 0; i < _meshes.size(); ++i ) {
        _meshes[ i ].DrawMesh();
    }
}


// geo container

CGeoContainer::CGeoContainer() : _inited( false ) { 
    for( us i = 0; i < GEO_COUNTER; ++i ) {
        _geos[ i ] = 0;
    }
}

void CGeoContainer::Init() {
    if( _inited ) {
        LogError<<"geo container already inited"<<LogEndl;
        return;
    }

    _geos[ GEO_TRIANGLE  ] = new CTriangleGeo();
    _geos[ GEO_CUBE  ] = new CCubeGeo();
    _geos[ GEO_SPHERE  ] = new CSphereGeo( true );
    _geos[ GEO_SPIDER  ] = new CSpiderGeo( true );

    _inited = true;
    
}

void CGeoContainer::Deinit() {
    if( !_inited ) {
        LogError<<"geo container not inited"<<LogEndl;
        return;
    }

    for( us i = 0 ; i < GEO_COUNTER; ++i ) {
        if( _geos[i] ) {
            delete _geos[i];
            _geos[i] = 0;
        }
    }

    _inited = false;
}

void CGeoContainer::DrawGeo( GEO_TYPE t_type, CShader* t_shader, CMaterial* t_material, const mat4& t_modelMatrix ) {
    if( !_inited ) {
        LogError<<"geo container not inited"<<LogEndl;
        return;
    }

    if( !_geos[ t_type ] ) {
        LogError<<"geo not inited in geo container"<<LogEndl;
        return;
    }

    _geos[ t_type ]->DrawModel( t_shader, t_material, t_modelMatrix );
}