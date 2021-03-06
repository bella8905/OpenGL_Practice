/////////////////////////////////////////////////////////////////
//
//  Practice_11_19_14 - object
//
//  Implementation of Anton's OpenGL tutorial
//
//  Copyright (c) 2014 Bella Q
//  
/////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <string>
#include "assimp/scene.h"
#include "Utl_Include.h"
#include "Material.h"

class CShader;

using std::vector;
using std::string;  

class CObject
{
public:
    CObject();
    virtual ~CObject() = 0;

protected:
    bool _inited;
/*    GLuint _vao;*/
    glm::mat4 _modelMat;
    float _scale;       // only allow uniform scale
//     glm::vec3 _translate;
//     glm::mat3 _rot;
    // CShader* _shader;

    CMaterial _material;
protected:
    virtual bool initModel();
    virtual void deinitModel() = 0; // delete buffer, etc
    // void calModelMat();

public:
    virtual void DrawModel( CShader* t_shader );
    bool IsInited() { return _inited; }

    mat4& GetModelMat() { return _modelMat; }
    void SetScales( const float& t_scales );
    float& GetScales() { return _scale; }

    void SetMaterial( const CMaterial& t_val) { _material = t_val; }
    CMaterial& GetMaterial() { return _material; }

};

struct SVertex {
    vec3 _pos;
    vec3 _normal;

    SVertex( const vec3& t_pos, const vec3& t_normal ) : _pos( t_pos ), _normal( t_normal ) {}
};


class CPrimitive : public CObject {
public:
    CPrimitive() : _vao( 0 ), _vbo( 0 ) {}
    ~CPrimitive() { deinitModel(); }

protected:
    // we need to keep track of vao loc so we can bind to it and draw our mesh later
    // but we also need to keep track of vbo loc so we can clear the buffer obj when we are done with this mesh
    // delete vao only removes the references to vbos
    // http://stackoverflow.com/questions/14274860/does-gldeletevertexarrays-lead-to-deletion-of-vbos-associated-with-vao-being-de
    GLuint _vao;
    GLuint _vbo;    // use a single buffer obj for pos and normal  
    GLuint _ibo;    // we'd like to use drawElement rather than drawArray. It saves space and is faster.
    int _numOfIndices;

protected:
    virtual bool initModel() = 0; 
    void deinitModel();

    void genBufferData( const vector<SVertex>& t_vertices, const vector<GLuint>& t_indices  );

public:
    virtual void DrawModel( CShader* t_shader );
};


// triangle
class CTriangle : public CPrimitive {
public:
    CTriangle() { initModel(); }
    ~CTriangle() { deinitModel(); }

protected:
    bool initModel();
};

// cube
class CCube : public CPrimitive {
public:
    CCube() { initModel(); }
    ~CCube() { deinitModel(); }

protected:
    bool initModel();
};



// a object read from a model file
class CModel : public CObject {
public:
    CModel( const string& t_file, bool t_unified = false ) :  _fileName( t_file ), _unified( t_unified ) { initModel(); }
    ~CModel() { deinitModel(); }

    struct SBoundBox {
        vec3 _min, _max;

        void SetBounds( const vec3& t_point ) {
            _min.x = ( t_point.x < _min.x ) ? t_point.x : _min.x;
            _min.y = ( t_point.y < _min.y ) ? t_point.y : _min.y;
            _min.z = ( t_point.z < _min.z ) ? t_point.z : _min.z;
            _max.x = ( t_point.x > _max.x ) ? t_point.x : _max.x;
            _max.y = ( t_point.y > _max.y ) ? t_point.y : _max.y;
            _max.z = ( t_point.z > _max.z ) ? t_point.z : _max.z;
        }

        void SetBounds( const SBoundBox& t_bounds ) {
            _min.x = min( _min.x, t_bounds._min.x);
            _min.y = min( _min.y, t_bounds._min.y);
            _min.z = min( _min.z, t_bounds._min.z);
            _max.x = max( _max.x, t_bounds._max.x);
            _max.y = max( _max.y, t_bounds._max.y);
            _max.z = max( _max.z, t_bounds._max.z);
        }

        vec3 GetCenter() {
            return ( _min + _max ) * 0.5f;
        }

        float GetLongestSide() {
            float length = -1;
            for( int i = 0; i < 3; ++i ) {
                float sideLength = _max[i] - _min[i];
                length = max( sideLength, length );
            }

            return length;
        }
    };

    struct SMesh {
        // don't have reference to ass scene, the scene is already freed at mesh destructor time
        // const aiMesh* _aiMesh;
        int _numOfIndices;

        // generate buffer for each of the vertex attribute
        // this will save more space than create a vertext struct and transport all things to GPU
        // for sometimes we don't have texcoords or color for a vertex
        GLuint _vao, _vbo, _nbo, _tbo, _ibo;
        bool _hasTex, _hasFaces;
        bool _inited;
        SBoundBox _bounds;

        SMesh() : _inited( false ), _numOfIndices( 0 ), 
                  _vao( 0 ), _vbo( 0 ), _nbo( 0 ), _tbo( 0 ), _ibo( 0 ), 
                  _hasTex( false ), _hasFaces( false ) {}

        void InitMesh( const aiMesh* t_aiMesh, bool t_unitScale = false );
        void DeinitMesh();
        void DrawMesh();
    };

protected:
    string _fileName;
    vector<SMesh> _meshes;
    bool _unified;       // if the model is scaled to fit a unit cube
    // scale and translate the model to fit in a unit cube centered at origin if _unified is set
    // these are not the real transformations in model matrix
    float _adjustedScale;    
    vec3  _adjustedTranslate;

protected:
    bool initModel();
    void deinitModel();

public:
    virtual void DrawModel( CShader* t_shader );
};