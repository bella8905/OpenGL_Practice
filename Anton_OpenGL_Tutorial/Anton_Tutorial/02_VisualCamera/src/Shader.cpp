#include <iostream>
#include <fstream>
#include <streambuf>

#include "Shader.h"
#include "Utl_Include.h"

// CShader::CShader( const std::string& t_vs, const std::string& t_fs ) : CShader(  t_vs, "", "", t_fs ) {
// }

CShader::CShader( const std::string& t_vs, const std::string& t_fs, const std::string& t_gs, const std::string& t_ts ) : _vs( t_vs ), _gs( t_gs ), _ts( t_ts ), _fs( t_fs ), _inited( false ), _sp(0)  {
    initSP();
}


CShader::~CShader()
{
}


///////////////////////////////////////////////
// Shader / Shader Program Error Checking

// read in txt file as std std::strings
bool CShader::readFileToStr( const char* t_file, std::string& t_fileContentStr  ) {
    ifstream in( t_file );
    if( !in || in.bad() ) {
        LogError<<"read shader file <<"<<t_file<<">> failed"<<LogEndl;
        in.close();
        in.clear();
        return false;
    }

    t_fileContentStr = std::string( ( istreambuf_iterator<char>( in ) ), istreambuf_iterator<char>() );

    in.close();
    in.clear();

    return true;
}


std::string CShader::glTypeToString( GLenum t_type ) {
    switch( t_type ) {
    case GL_BOOL: return "bool";
    case GL_INT: return "int";
    case GL_FLOAT: return "float";
    case GL_FLOAT_VEC2: return "vec2";
    case GL_FLOAT_VEC3: return "vec3";
    case GL_FLOAT_VEC4: return "vec4";
    case GL_FLOAT_MAT2: return "mat2";
    case GL_FLOAT_MAT3: return "mat3";
    case GL_FLOAT_MAT4: return "mat4";
    case GL_SAMPLER_2D: return "sampler2D";
    case GL_SAMPLER_3D: return "sampler3D";
    case GL_SAMPLER_CUBE: return "samplerCube";
    case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
    default: break;
    }
    return "other";
}

// print errors in shader compilation
void CShader::printShaderInfoLog( const GLuint& t_shaderIndex ) {
    int maxLength = 2048;
    int actualLength = 0;
    char log[2048];
    glGetShaderInfoLog( t_shaderIndex, maxLength, &actualLength, log );
    LogMsg<<"shader info log for GL index "<<t_shaderIndex<<LogEndl<<log<<LogEndl<<LogEndl;
}

// print errors in shader linking
void CShader::printSPInfoLog( const GLuint& t_spIndex) {
    int maxLength = 2048;
    int actualLength = 0;
    char log[2048];
    glGetProgramInfoLog (t_spIndex, maxLength, &actualLength, log);
    LogMsg<<"program info log for GL index "<<t_spIndex<<LogEndl<<log<<LogEndl<<LogEndl;
}


bool CShader::checkShaderCompileStatus( const GLuint& t_shaderIndex ) {
    int param = -1;
    glGetShaderiv( t_shaderIndex, GL_COMPILE_STATUS, &param );
    if( GL_TRUE != param ) {
        LogError<<"GL shader index "<<t_shaderIndex<<" did not compile"<<LogEndl;
        printShaderInfoLog( t_shaderIndex );
        return false;
    }
    return true;
}

bool CShader::checkSPLinkingStatus( const GLuint& t_spIndex ) {
    int param = -1;
    glGetProgramiv( t_spIndex, GL_LINK_STATUS, &param );
    if( GL_TRUE != param ) {
        LogError<<"GL shader program index "<<t_spIndex<<" did not link successfully"<<LogEndl;
        printSPInfoLog( t_spIndex );
        return false;
    }
    return true;
}

bool CShader::validateSP( const GLuint& t_spIndex ) {
    int param = -1;
    glValidateProgram( t_spIndex );
    glGetProgramiv( t_spIndex, GL_VALIDATE_STATUS, &param );
    LogMsg<<"program "<<t_spIndex<<" GL_VALIDATE_STATUS = " << param<<LogEndl;
    if (GL_TRUE != param) {
        printSPInfoLog( t_spIndex );
        return false;
    }
    return true;
}

// print absolutely everything about a shader program - only useful if you get really
// stuck wondering why a shader isn't working properly 
void CShader::printSPInfo( const GLuint& sp) {
    int params = -1;
    int i;

    LogMsg<<"--------------------shader program "<<sp<<" info--------------------"<<LogEndl;
    glGetProgramiv( sp, GL_LINK_STATUS, &params);
    LogMsg<<"GL_LINK_STATUS = "<<params<<LogEndl;

    glGetProgramiv( sp, GL_ATTACHED_SHADERS, &params);
    LogMsg<<"GL_ATTACHED_SHADERS = "<<params<<LogEndl;


    glGetProgramiv( sp, GL_ACTIVE_ATTRIBUTES, &params);
    LogMsg<<"GL_ACTIVE_ATTRIBUTES = "<<params<<LogEndl;

    for( i = 0; i < params; ++i) {
        char name[64];
        int maxLength = 64;
        int actualLength = 0;
        int size = 0;
        GLenum type;
        glGetActiveAttrib( sp, i, maxLength, &actualLength, &size, &type, name);
        if( size > 1) {
            int j;
            for( j = 0; j < size; ++j) {
                ostringstream ss;
                int location;
                ss<<name<<"["<<j<<"]";
                std::string longName = ss.str();
                location = glGetAttribLocation(sp, longName.c_str() );
                LogMsg<<i<<"i) type: "<<glTypeToString( type )<<", name: "<<longName<<", location: "<<location<<LogEndl;
            }
        } else {
            int location = glGetAttribLocation( sp, name );
            LogMsg<<i<<") type: "<<glTypeToString( type )<<", name: "<<name<<", location: "<<location<<LogEndl;
        }
    }

    glGetProgramiv( sp, GL_ACTIVE_UNIFORMS, &params);
    LogMsg<<"GL_ACTIVE_UNIFORMS = "<<params<<LogEndl;
    for( i = 0; i < params; ++i) {
        char name[64];
        int maxLength = 64;
        int actualLength = 0;
        int size = 0;
        GLenum type;
        glGetActiveUniform( sp, i, maxLength, &actualLength, &size, &type, name);
        if( size > 1) {
            int j;
            for( j = 0; j < size; ++j) {
                ostringstream ss;
                int location;
                ss<<name<<"["<<j<<"]";
                std::string longName = ss.str();
                location = glGetUniformLocation( sp, longName.c_str() );
                LogMsg<<i<<"i) type: "<<glTypeToString( type )<<", name: "<<longName<<", location: "<<location<<LogEndl;
            }
        } else {
            int location = glGetUniformLocation( sp, name);
            LogMsg<<i<<") type: "<<glTypeToString( type )<<", name: "<<name<<", location: "<<location<<LogEndl;
        }
    }

    printSPInfoLog( sp );
}

GLint CShader::createShader( const char* t_shaderFile, GLenum t_shaderType ) {
    std::string shaderContent;
    if( !readFileToStr( t_shaderFile, shaderContent ) ) return -1;

    const GLchar* p = 0;

    GLuint shaderIndex = glCreateShader( t_shaderType );
    p = (const GLchar*)shaderContent.c_str();
    glShaderSource( shaderIndex, 1, &p, NULL );
    glCompileShader( shaderIndex );
    if( !checkShaderCompileStatus( shaderIndex ) )  return -1;

    return shaderIndex;

}

GLuint CShader::createShaderProgram( const GLint& t_vs, const GLint& t_gs, const GLint& t_ts, const GLint& t_fs  ) {
    GLuint sp = glCreateProgram();
    glAttachShader( sp, t_vs );
    glAttachShader( sp, t_fs );
    if( t_gs >= 0 ) glAttachShader( sp, t_gs );
    if( t_ts >= 0 ) glAttachShader( sp, t_ts );

    glLinkProgram ( sp );
    if( !checkSPLinkingStatus( sp ) ) return -1;
    assert( validateSP( sp ) );

    // delete shaders here to free memory
    // https://www.opengl.org/discussion_boards/showthread.php/182721-Does-glDeleteProgram-free-shaders
    glDetachShader( sp, t_vs );
    glDetachShader( sp, t_fs ); 
    glDeleteShader( t_vs );
    glDeleteShader( t_fs );

    if( t_gs >= 0 ) {
        glDetachShader( sp, t_gs ); 
        glDeleteShader( t_gs );
    }
    if( t_ts >= 0 ) {
        glDetachShader( sp, t_ts ); 
        glDeleteShader( t_ts );
    }

    return sp;
}

// bind shaders by vertex shader file and fragment shader fie
void CShader::createShaderProgram() {
    GLint vs = -1;
    GLint gs = -1;
    GLint ts = -1;
    GLint fs = -1;

    vs = createShader( _vs.c_str(), GL_VERTEX_SHADER );
    fs = createShader( _fs.c_str(), GL_FRAGMENT_SHADER );
    assert(  vs >= 0 );
    assert(  fs >= 0 );

    if( _gs!= "" ) {
        gs = createShader( _gs.c_str(), GL_GEOMETRY_SHADER );
        assert( gs >= 0 );
    }

    // bypass tessellation shader for now
    if( _ts != "" ) {
        // ts = createShader( _ts.c_str(), gl_tessshader)
    }

    _sp = createShaderProgram( vs, gs, ts, fs );  
    
    _inited = true;
}

void CShader::BindShader()
{
    if( !_inited ) {
        LogError<<"shader not inited"<<LogEndl;
        return;
    }

    glUseProgram( _sp );
}

void CShader::initSP() {
    createShaderProgram();
}



const std::string CPerspCamShader::_VS_FILE = "shaders/lookAtCam.vert";
const std::string CPerspCamShader::_FS_FILE = "shaders/simple.frag";

CPerspCamShader::CPerspCamShader( CCamera* t_cam ) : CShader( _VS_FILE, _FS_FILE ), _camera( t_cam ), _vertexColor( vec4( 1.0f, 0.0f, 0.0f, 1.0f ) )
{
}

void CPerspCamShader::initSP() {
    assert( _camera != 0 );

    CShader::initSP();
    
    // uniforms
    _uni_inputColorLoc = glGetUniformLocation( _sp, "inputColor" );
    _uni_viewMatLoc = glGetUniformLocation( _sp, "view" );
    _uni_projMatLoc = glGetUniformLocation( _sp, "proj" );
    assert( _uni_inputColorLoc >= 0 && _uni_projMatLoc >= 0 && _uni_viewMatLoc >= 0 );

    // attributes
    _attr_pos = 0;

    _inited = true;
}

void CPerspCamShader::BindShader() {

    if( !_inited ) {
        LogError<<"shader not inited"<<LogEndl;
        return;
    }

    CShader::BindShader();

    glUniform4fv( _uni_inputColorLoc, 1, glm::value_ptr( _vertexColor ) );
    glUniformMatrix4fv( _uni_viewMatLoc, 1, GL_FALSE, glm::value_ptr( _camera->GetViewMat() ) );
    glUniformMatrix4fv( _uni_projMatLoc, 1, GL_FALSE, glm::value_ptr( _camera->GetProjMat() ) );
}