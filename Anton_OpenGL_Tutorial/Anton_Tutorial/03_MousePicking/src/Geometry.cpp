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

GLuint CGeo::_vao_boundBox = 0;
GLuint CGeo::_vbo_boundBox = 0;
GLuint CGeo::_ibo_boundBox = 0;
us CGeo::_numOfIndices_boundBox = 0;
bool CGeo::_inited_boundBox = false;  

CGeo::CGeo() : _inited( false ), _preprocessModelMatrix( 1.f ) {
}


CGeo::~CGeo(void)
{
}


void CGeo::InitBoundBox() {
    if( _inited_boundBox ) {
        LogError<<"bound box already inited" <<LogEndl;
        return;
    }

    // get boundbox vertices from _boundbox
    // int bound box vbo and ibo
    GLfloat cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f, 
        0.5f,  0.5f, -0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        0.5f, -0.5f,  0.5f, 
        0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 
    };


    GLushort cubeIndices [] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    glGenVertexArrays( 1, &_vao_boundBox );
    glBindVertexArray( _vao_boundBox  );


    glGenBuffers( 1, &_vbo_boundBox );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_boundBox );
    glBufferData( GL_ARRAY_BUFFER, sizeof( cubeVertices ), cubeVertices, GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindVertexArray( 0 );

    glGenBuffers( 1, &_ibo_boundBox );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo_boundBox);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( cubeIndices ), cubeIndices, GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    _inited_boundBox = true;
}

void CGeo::DeinitBoundBox() {
    if( !_inited_boundBox ) {
        LogError<<"bound box not inited"<<LogEndl;
        return;
    }

    glDeleteBuffers( 1, &_vbo_boundBox );
    glDeleteBuffers( 1, &_ibo_boundBox );

    _inited_boundBox = false;
}

bool CGeo::initModel() {
    if( _inited ) return true;

    return _inited;
}

void CGeo::deinitModel()  {
    if( !_inited )  return;
}

void CGeo::DrawModel( SHADER_TYPE t_shader, CMaterial* t_material, const mat4& t_modelMatrix, bool t_drawBB ) {
    if( !_inited ) {
        LogError<<"model not inited"<<LogEndl;
        return;
    }

    // bound box
    if( t_drawBB && _inited_boundBox ) {
        // bind bound box shader and draw bind box
        // don't need object and material for single color shader
        // but we need to recalcuate the bound box model matrix
        // scale and then translate the unit box to transform it to bb
        mat4 scaleMat = mat4(   vec4( _boundBox._sideLentghs[0], 0.f,            0.f,            0.f ), 
                                vec4( 0.f,            _boundBox._sideLentghs[1], 0.f,            0.f ), 
                                vec4( 0.f,            0.f,            _boundBox._sideLentghs[2], 0.f ), 
                                vec4( 0.f,            0.f,            0.f,            1.f ) );

        mat4 translateMat = mat4(   vec4( 1.f, 0.f, 0.f, 0.f ),
                                    vec4( 0.f, 1.f, 0.f, 0.f ), 
                                    vec4( 0.f, 0.f, 1.f, 0.f ), 
                                    vec4( _boundBox._center, 1.f ) );

        mat4 bbTfmMat( 1.f );
        bbTfmMat = t_modelMatrix * translateMat * scaleMat;
        
        glBindVertexArray( _vao_boundBox );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo_boundBox );


        CShaderContainer::GetInstance().BindShaderForDrawing( SD_SINGLE_COLOR, 0, 0, bbTfmMat );
        glDrawElements( GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, NULL );
        glDrawElements( GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, ( GLvoid* )( 4 * sizeof( GLushort ) ) );
        glDrawElements( GL_LINES, 8, GL_UNSIGNED_SHORT, ( GLvoid* )( 8*sizeof( GLushort ) ) );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
    }

    CShaderContainer::GetInstance().BindShaderForDrawing( t_shader, this, t_material, t_modelMatrix );
}

// primitive
bool CPrimGeo::initModel() {
    return CGeo::initModel();
}

void CPrimGeo::deinitModel() {
    if( !_inited )  return;
    glDeleteBuffers( 1, &_vbo );
    glDeleteBuffers( 1, &_ibo );

    CGeo::deinitModel();
}

void CPrimGeo::DrawModel( SHADER_TYPE t_shader, CMaterial* t_material, const mat4& t_modelMatrix, bool t_drawBB ) {

    CGeo::DrawModel( t_shader, t_material, t_modelMatrix, t_drawBB );

    glBindVertexArray( _vao );
//     glBindBuffer( GL_ARRAY_BUFFER, _vbo );
//     glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( SVertex ), (void*)offsetof( SVertex, _pos ) );
//     glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( SVertex ), (void*)offsetof( SVertex, _normal ) );
//     glEnableVertexAttribArray( 0 );
//     glEnableVertexAttribArray( 1 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );

    glDrawElements( GL_TRIANGLES, _numOfIndices, GL_UNSIGNED_INT, NULL );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
    // glBindBuffer( GL_ARRAY_BUFFER, 0 );

}

void CPrimGeo::genBufferData( const vector<SVertex>& t_vertices, const vector<GLuint>& t_indices ) {

    // generate vao and vbos
    glGenVertexArrays( 1, &_vao );
    glBindVertexArray( _vao );

    glGenBuffers( 1, &_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( SVertex ) * t_vertices.size(), &t_vertices[0], GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( SVertex ), (void*)offsetof( SVertex, _pos ) );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( SVertex ), (void*)offsetof( SVertex, _normal ) );
    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindVertexArray( 0 );

    glGenBuffers( 1, &_ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, _numOfIndices * sizeof( GLuint ), &t_indices[0], GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );


}

// triangle
bool CTriangleGeo::initModel() {
    LogMsg<<"Init Triangle"<<LogEndl;
    // if obj is already inited, simply return
    if( CPrimGeo::initModel() ) return true;

    vector<SVertex> vertices;
    vec3 point = vec3( 0.0f, 0.5f, 0.0f );
    vec3 normal = vec3( 0.f, 0.f, 1.f );
    vertices.push_back( SVertex( point, normal ) );
    _boundBox.SetBounds( point );

    point = vec3( -0.5f, -0.5f, 0.0f );
    vertices.push_back( SVertex( point, normal ) );
    _boundBox.SetBounds( point );

    point = vec3( 0.5f, -0.5f, 0.0f );
    vertices.push_back( SVertex( point, normal ) );
    _boundBox.SetBounds( point );

    // give bb a little thickness
    _boundBox.Validate();

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

        _boundBox.SetBounds( vec3( cubeVertices[ startIndex ], cubeVertices[ startIndex + 1 ], cubeVertices[ startIndex + 2 ] ) );
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

            _bounds.SetBounds( vec3( point->x, point->y, point->z ) );
        }

        glGenBuffers( 1, &_vbo );
        glBindBuffer( GL_ARRAY_BUFFER, _vbo );
        glBufferData( GL_ARRAY_BUFFER, 3 * numOfVertices * sizeof( GLfloat ), &points[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

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
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

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
        glBindBuffer( GL_ARRAY_BUFFER, 0 );


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
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        
/*    }*/

    glBindVertexArray( 0 );

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
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
    glDrawElements( GL_TRIANGLES, _numOfIndices, GL_UNSIGNED_INT, NULL );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
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

        bounds.SetBounds( mesh._bounds );
    }

    _boundBox = bounds;

    if( _unified ) {
        vec3 adjustedTranslate = -bounds._center;
        float adjustedScale = 2.f / bounds.GetLongestSide();

        mat4 scaleMat = mat4(   vec4( adjustedScale, 0.f,            0.f,            0.f ), 
                                vec4( 0.f,            adjustedScale, 0.f,            0.f ), 
                                vec4( 0.f,            0.f,            adjustedScale, 0.f ), 
                                vec4( 0.f,            0.f,            0.f,            1.f ) );

        mat4 translateMat = mat4(   vec4( 1.f, 0.f, 0.f, 0.f ),
                                    vec4( 0.f, 1.f, 0.f, 0.f ), 
                                    vec4( 0.f, 0.f, 1.f, 0.f ), 
                                    vec4( adjustedTranslate, 1.f ) );

        // translate the model first to center and then scale
        _preprocessModelMatrix = _preprocessModelMatrix * scaleMat * translateMat;
        _boundBox.Translate( adjustedTranslate );
        _boundBox.Scale( adjustedScale );
    }

    _boundBox.Validate();
 
    _inited = true;
    return _inited;
}

void CModelGeo::deinitModel() {
    if( !_inited ) return;
    for( unsigned int i = 0; i < _meshes.size(); ++i ) {
        _meshes[i].DeinitMesh();
    }

    CGeo::deinitModel();

    _inited = false;
}

void CModelGeo::DrawModel( SHADER_TYPE t_shader, CMaterial* t_material, const mat4& t_modelMatrix, bool t_drawBB ) {

    CGeo::DrawModel( t_shader, t_material, t_modelMatrix, t_drawBB );

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

    //  init bound box 
    CGeo::InitBoundBox();

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

    //  deinit bound box 
    CGeo::DeinitBoundBox();

    _inited = false;
}

SBoundBox* CGeoContainer::GetGeoBB( const GEO_TYPE& t_geoType ) {
    if( !_inited ) {
        LogError<<"geo container not inited"<<LogEndl;
        return 0;
    }

    if( !_geos[ t_geoType ] ) {
        LogError<<"geo not inited in geo container"<<LogEndl;
        return 0;
    }

    return &( _geos[ t_geoType ]->GetBB() );
}

void CGeoContainer::DrawGeo( const GEO_TYPE& t_geoType, const SHADER_TYPE& t_shaderType, CMaterial* t_material, const mat4& t_modelMatrix, const bool& t_drawBB ) {
    if( !_inited ) {
        LogError<<"geo container not inited"<<LogEndl;
        return;
    }

    if( !_geos[ t_geoType ] ) {
        LogError<<"geo not inited in geo container"<<LogEndl;
        return;
    }

    _geos[ t_geoType ]->DrawModel( t_shaderType, t_material, t_modelMatrix, t_drawBB );
}