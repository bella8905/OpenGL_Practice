#pragma once

#include <vector>
#include <string>
#include "assimp/scene.h"
#include "Utl_Include.h"
#include "Shader.h"

using std::vector;
using std::string;

class CObject
{
public:
    CObject( CShader* t_shader );
    virtual ~CObject() = 0;

protected:
    bool _inited;
/*    GLuint _vao;*/
    CShader* _shader;
    glm::mat4 _modelMat;
    glm::vec3 _scale;
    glm::vec3 _translate;
    glm::mat3 _rot;

protected:
    virtual bool initModel() = 0;
    virtual void deinitModel() = 0; // delete buffer, etc
    void calModelMat();

public:
    virtual void DrawModel() = 0;
    bool IsInited() { return _inited; }

    mat4& GetModelMat() { return _modelMat; }
    void SetScales( const vec3& t_scales );
    vec3& GetScales() { return _scale; }

};



class CTriangle : public CObject {
public:
    CTriangle( CShader* t_shader  ) : CObject( t_shader ),  _vao( 0 ) { initModel(); }
    ~CTriangle() { deinitModel(); }

protected:
    // we need to keep track of vao loc so we can bind to it and draw our mesh later
    // but we also need to keep track of vbo loc so we can clear the buffer obj when we are done with this mesh
    // delete vao only removes the references to vbos
    // http://stackoverflow.com/questions/14274860/does-gldeletevertexarrays-lead-to-deletion-of-vbos-associated-with-vao-being-de
    GLuint _vao, _vbo;  
protected:
    bool initModel();
    void deinitModel();

public:
    virtual void DrawModel();
};


// a object read from a model file
class CModel : public CObject {
public:
    CModel( CShader* t_shader, const string& t_file ) : CObject( t_shader ), _fileName( t_file ) { initModel(); }
    ~CModel() { deinitModel(); }

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

        SMesh() : _inited( false ), _numOfIndices( 0 ), 
                  _vao( 0 ), _vbo( 0 ), _nbo( 0 ), _tbo( 0 ), _ibo( 0 ), 
                  _hasTex( false ), _hasFaces( false ) {}

        void InitMesh( const aiMesh* t_aiMesh );
        void DeinitMesh();
        void DrawMesh();
    };

protected:
    string _fileName;
    vector<SMesh> _meshes;

protected:
    bool initModel();
    void deinitModel();

public:
    virtual void DrawModel();
};