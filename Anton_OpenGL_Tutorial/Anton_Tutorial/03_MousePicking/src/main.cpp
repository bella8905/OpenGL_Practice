/////////////////////////////////////////////////////////////////
//
//  Practice_11_19_14 - main
//
//  Implementation of Anton's OpenGL tutorial
//
//  Copyright (c) 2014 Bella Q
//  
/////////////////////////////////////////////////////////////////

#include "Utl_Include.h"
#include "Camera.h"
#include "Shader.h"
#include "Geometry.h"
#include "Light.h"
#include "Scene.h"
#include "stb_image/stb_image_write.h"
#include "assimp/scene.h"


#include <GL/AntTweakBar.h>

#include <iostream>
#include <fstream>
#include <streambuf>

using namespace std;

int g_winWidth = 640;
int g_winHeight = 480;

bool g_drawWireModel = false;

const string g_imageFilePrefix = "images/screenshot_"; 

///////////////////////////////////////////////
// GUI : AntTweakBar
// wrapper function for GLFW3 integration

void _gui_init( int t_winWidth, int t_winHeight ) {
    assert( t_winWidth );
    assert( t_winHeight );

    TwInit( TW_OPENGL_CORE, NULL );
    TwWindowSize( t_winWidth, t_winHeight );
}

void _gui_deinit() {
    TwTerminate();
}

void _gui_draw() {
    // don't let the wire thing affect menu
    if( g_drawWireModel ) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glEnable( GL_CULL_FACE );
    }

    TwDraw();

    if( g_drawWireModel ) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glDisable( GL_CULL_FACE );
    }
}

void _gui_onResize( int t_winWidth, int t_winHeight ) {
    TwWindowSize( t_winWidth, t_winHeight );
}

// carefully not using old GLFW2 functions in library
// https://gist.github.com/roxlu/8170860/download#

void _gui_onMouseClicked( int t_bt, int t_action ) {
    TwMouseButtonID btn = ( t_bt == GLFW_MOUSE_BUTTON_LEFT ) ? TW_MOUSE_LEFT : ( ( t_bt == GLFW_MOUSE_BUTTON_RIGHT ) ? TW_MOUSE_RIGHT : TW_MOUSE_MIDDLE );
    TwMouseAction ma = ( t_action == GLFW_PRESS ) ? TW_MOUSE_PRESSED : TW_MOUSE_RELEASED;
    TwMouseButton( ma, btn );
}

void _gui_onKeyPressed( int t_key, int t_mod ) {
    // direct keys. values have been changed from GLFW2 to GLFW3
    switch ( t_key ) {
    case GLFW_KEY_LEFT: t_key = TW_KEY_LEFT; break;
    case GLFW_KEY_RIGHT: t_key = TW_KEY_RIGHT; break;
    case GLFW_KEY_UP: t_key = TW_KEY_UP; break;
    case GLFW_KEY_DOWN: t_key = TW_KEY_DOWN; break;
//     case GLFW_KEY_BACKSPACE: t_key = TW_KEY_BACKSPACE; break;
//     case GLFW_KEY_ENTER: t_key = TW_KEY_RETURN; break;
    default: break;
    }

    int tw_mod = TW_KMOD_NONE;
    if( t_mod & GLFW_MOD_SHIFT ) {
        tw_mod |= TW_KMOD_SHIFT;
    }
    if( t_mod & GLFW_MOD_CONTROL ) {
        tw_mod |= TW_KMOD_CTRL;
    }
    if( t_mod & GLFW_MOD_ALT ) {
        tw_mod |= TW_KMOD_ALT;
    }
    
    // this one is disaster
    // should come back later and read the library code
    // but for now, keep it as the minimal function
    TwEventKeyGLFW( t_key, tw_mod );
    // TwKeyPressed( t_key, tw_mod );   // react multiple times ..
}

void _gui_mouseButtonCallback( GLFWwindow* t_window, int t_btn, int t_action, int t_mods ) {
    _gui_onMouseClicked( t_btn, t_action );
}

void _gui_mouseMoveCallback( GLFWwindow* t_window, double t_x, double t_y ) {
    TwEventMousePosGLFW( ( int )t_x, ( int )t_y );
}

void _gui_mouseScrollCallback( GLFWwindow* t_window, double t_xoffset, double t_yoffset ) {
    TwEventMouseWheelGLFW( ( int )t_yoffset );
}

void _gui_keyCallback( GLFWwindow* t_window, int t_key, int t_scancode, int t_action, int t_mods ) {
    _gui_onKeyPressed( t_key, t_mods );
}

 void _gui_charCallback( GLFWwindow* t_window, int t_codepoint ) {
    TwEventCharGLFW( t_codepoint, GLFW_PRESS );
 }



// GUI : AntTweakBar
///////////////////////////////////////////////

void _glfwErrorCallback( int t_error, const char* t_desc ) {
    LogError<<"GLFW ERROR: code "<<t_error<<" msg: "<<t_desc<<LogEndl;
}

void _glfwWindowSizeCallback( GLFWwindow* t_win, int t_width, int t_height ) {
    g_winWidth = t_width;
    g_winHeight = t_height;

    // Send the new window size to AntTweakBar
    _gui_onResize( t_width, t_height);
}


void _logGLParams() {
    enum ParamValType { SINGLE_INT, VEC_INT, SINGLE_BOOL, };
    
    struct glPrama {
        GLenum _param;
        string _paramStr;
        ParamValType _valType;
    } glParams[] = {
        // https://www.opengl.org/sdk/docs/man4/
        { GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,  "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",  SINGLE_INT },
        { GL_MAX_CUBE_MAP_TEXTURE_SIZE,         "GL_MAX_CUBE_MAP_TEXTURE_SIZE",         SINGLE_INT },
        // max count of buffer, that rendering in same time.
        // for more advanced effects where we want to split the output from our rendering into different images
        { GL_MAX_DRAW_BUFFERS,                  "GL_MAX_DRAW_BUFFERS",                  SINGLE_INT },
        { GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,   "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",   SINGLE_INT },
        { GL_MAX_TEXTURE_IMAGE_UNITS,           "GL_MAX_TEXTURE_IMAGE_UNITS",           SINGLE_INT },
        { GL_MAX_TEXTURE_SIZE,                  "GL_MAX_TEXTURE_SIZE",                  SINGLE_INT },
        // Varying floats are those sent from the vertex shader to the fragment shaders. 
        // NOT SURE WHY VARYING_FLOATS IS INVALID ENUM ON MY DESKTOP. TRY IT LATER SOMEWHERE ELSE
        // { GL_MAX_VARYING_FLOATS,                "GL_MAX_VARYING_FLOATS",                SINGLE_INT },
        { GL_MAX_VERTEX_ATTRIBS,                "GL_MAX_VERTEX_ATTRIBS",                SINGLE_INT },
        { GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,    "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",    SINGLE_INT },
        { GL_MAX_VERTEX_UNIFORM_COMPONENTS,     "GL_MAX_VERTEX_UNIFORM_COMPONENTS",     SINGLE_INT },
        { GL_MAX_VIEWPORT_DIMS,                 "GL_MAX_VIEWPORT_DIMS",                 VEC_INT },
        { GL_STEREO,                            "GL_STEREO",                            SINGLE_BOOL },
    };

    us count = sizeof( glParams ) / sizeof( glPrama );
    LogMsg<<"GL Context Params: "<<LogEndl;
    for( us i = 0 ; i < count; ++i  ) {
        LogMsg<<glParams[ i ]._paramStr<<": ";
        switch( glParams[ i ]._valType )
        {
        case SINGLE_INT:
            {
                int v = 0;
                glGetIntegerv( glParams[ i ]._param, &v );
                LogMsg<<v;
            } break;
        case VEC_INT:
            {
                int v[2];
                glGetIntegerv( glParams[ i ]._param, v );
                LogMsg<<v[0]<<" "<<v[1];
            } break;
        case SINGLE_BOOL:
            {
                unsigned char s = 0;
                glGetBooleanv( glParams[ i ]._param, &s );
                LogMsg<<(us)s;
            } break;
        default: break;
        }
        LogMsg<<LogEndl;
    }
    LogMsg<<LogEndl;
}

void _updateFPSCounter( GLFWwindow* t_window ) {
    if( t_window == 0  ) return;
    static double prevSeconds = glfwGetTime();
    static int frameCount = 0;
    double curSeconds = glfwGetTime();
    double elapsedSeconds = curSeconds - prevSeconds;
    if( elapsedSeconds > 0.25 ) {
        prevSeconds = curSeconds;
        double fps = ( double )frameCount / elapsedSeconds;
        ostringstream ss;
        ss<<"frame rate: "<<fps;
        string fpsStr = ss.str();
        glfwSetWindowTitle( t_window, fpsStr.c_str() );

        frameCount = 0;
    }

    frameCount++;
}


// ui callbacks
// function called when camera pos is changed in the tweakbar
// this function is only called when value in gui changed.
// so we can expect some benefits than brutally update view matrix every frame
void TW_CALL _setCameraPosCB( const void* t_value, void* t_clientData ) {
    CSimpleCamera* cam = ( CSimpleCamera* )t_clientData;
    if( cam == 0 )   return;

    vec3 newPos( ( (float*)t_value )[0],  ( (float*)t_value )[1],  ( (float*)t_value )[2] ); 
    cam->SetPos( newPos );

}

// function called when we get the camera pos to show in tweakbar
void TW_CALL _getCameraPosCB( void* t_value, void* t_clientData ) {
    CCamera* cam = ( CCamera* )t_clientData;
    if( cam == 0 )    return;

    memcpy( t_value, &( cam->GetPos().x ), 3 * sizeof( float ) );
}


void TW_CALL _setWireModeCB(  const void* t_value, void* t_clientData ) {
    g_drawWireModel = *(bool*)t_value;
    if( g_drawWireModel ) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable( GL_CULL_FACE ); 
    }
    else {
        // cull back face
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable( GL_CULL_FACE ); 
    }
}

void TW_CALL _getWireModeCB(  void* t_value, void* t_clientData ) {
    *(bool*)t_value = g_drawWireModel;
}

// screen capture
void _screenPrint() {
    string time = Utl::GetTime( Utl::TIME_STAMP_FILE_NAME );
    string imagefile = g_imageFilePrefix + time + ".png";

    ul size = g_winHeight * g_winWidth * 3;

    // dynamic memory is fine, cause we may change the viewport size
    // or better, we can pre allocate a large enough memory 
    // for video capture, we'd better do memory allocation first
    unsigned char* buffer = new unsigned char[ size ];
    glReadPixels( 0, 0, g_winWidth, g_winHeight, GL_RGB, GL_UNSIGNED_BYTE, buffer );
    unsigned char* lastRow = buffer + ( g_winWidth * 3 * ( g_winHeight - 1 ) );
    if( !stbi_write_png( imagefile.c_str(), g_winWidth, g_winHeight , 3, lastRow, -3 * g_winWidth ) ) {
        LogError<<"can't write to image"<<LogEndl;
    }

    delete[] buffer;
    LogPass<<"image saved"<<LogEndl;
}

int main()
{
    glfwSetErrorCallback( _glfwErrorCallback );

    if( !glfwInit() ) {
        LogError<< "could not start GLFW" << LogEndl;
        return 1;
    }


    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
    // disable all of the functionality from previous versions of OpenGL that has been marked as removal in the future
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    // mark functions deprecation
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_SAMPLES, 4 );

    GLFWwindow* window = glfwCreateWindow( g_winWidth, g_winHeight, "Hello Triangle", NULL, NULL );
    if( !window ) {
        LogError<< "could not open GLFW window" <<LogEndl;
        glfwTerminate();
        return 1;
    }

    glfwSetWindowSizeCallback( window, _glfwWindowSizeCallback );
    glfwMakeContextCurrent( window );
    // this should come before anything gl does
    // we should have some kind of context first

    // GLEW obtains information on the supported extensions from the graphics driver. 
    // Experimental or pre-release drivers, however, might not report every available extension through the standard mechanism, 
    // in which case GLEW will report it unsupported. To circumvent this situation, 
    // the glewExperimental global switch can be turned on by setting it to GL_TRUE before calling glewInit(), 
    // which ensures that all extensions with valid entry points will be exposed.
    glewExperimental = GL_TRUE;
    glewInit();

    // log version info
    const GLubyte* renderer = glGetString( GL_RENDERER );
    const GLubyte* version = glGetString( GL_VERSION );
    LogMsg<< "Renderer: " << renderer<<LogEndl;
    LogMsg<< "OpenGL version supported "<< version<<LogEndl<<LogEndl;

    // log monitors info
    int count;
    GLFWmonitor** monitors = glfwGetMonitors( &count );
    LogMsg<<"Monitors connected:"<<LogEndl;
    for( int i = 0; i < count; ++i ) {
        GLFWmonitor* mon = monitors[ i ];
        LogMsg<<i<<": "<<glfwGetMonitorName( mon );
        const GLFWvidmode* vmode = glfwGetVideoMode( mon );
        LogMsg<<", width: "<<vmode->width<<", height: "<<vmode->height<<"."<<LogEndl;
    }
    LogMsg<<LogEndl;

    // log context info
    _logGLParams();


    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );


    ////////////////////////////////////////////////////////
    // init scenes
    // camera
    // view matrix
    glm::vec3 camPos( 0.f, 0.f, 2.0f );
    glm::vec3 camUp( 0.f, 1.0f, 0.f );
    glm::vec3 camTarget( 0.f, 0.f, 0.f );
    glm::vec3 camFace = glm::normalize( camTarget - camPos );
    float clipNear = 0.1f;
    float clipFar = 100.f;
    // vertical fov
    float clipFOV = 60.f;
    float clipAspect = ( float )g_winWidth / ( float )g_winHeight;

    CSimpleCamera simpleCam;
    simpleCam.Setup( camPos, camFace, camUp, clipNear, clipFar, clipFOV, clipAspect );

    // light
    vec3 lightPos( 0.f, 0.f, 2.f );
    vec3 lightLs( 1.f, 1.f, 1.f );
    vec3 lightLd( 0.7f, 0.7f, 0.7f );
    vec3 lightLa( 0.2f, 0.2f, 0.2f );

    CLight simpleLight( lightPos, lightLs, lightLd, lightLa );

    // Shader 
    CPerspCamShader simpleShader( &simpleCam );

    // phong shader
    CPhongShader phongShader( &simpleCam );
    phongShader.SetLight( &simpleLight );

    // normal test shader
    CTestNormalShader testNormalShader( &simpleCam  );

    // material
    bool hasSpecular = true;
    Utl::CColor ks( 1.f, 1.f, 1.f );
    float specularExp = 10.f;
    Utl::CColor kd( 1.f, 0.5f, 0.f );
    Utl::CColor ka( 1.f, 1.f, 1.f );

    CMaterial blinnMat( kd, hasSpecular, ks, specularExp, ka );

    // geos
    CGeoContainer geos = CGeoContainer::GetInstance();
    geos.Init();
    

    mat4 left = Utl::GetModelMatFromTfms( vec3( -0.8f, 0.f, 0.f ), vec3( 0.f, 0.f, 0.f ), vec3( 0.3f, 0.3f, 0.3f ) );
    mat4 center = Utl::GetModelMatFromTfms( vec3( 0, 0.f, 0.f ), vec3( 0.f, 0.f, 0.f ), vec3( 0.3f, 0.3f, 0.3f ) );
    mat4 right = Utl::GetModelMatFromTfms( vec3( 0.8f, 0.f, 0.f ), vec3( 0.f, 0.f, 0.f ), vec3( 0.3f, 0.3f, 0.3f ) );
    CScene testScene;
    // cube 
    CObj obj_cube( GEO_CUBE );
    obj_cube._modelMat = left;
    obj_cube._shader = &testNormalShader;
    testScene.AddObj( obj_cube );

    CObj obj_sphere( GEO_SPHERE );
    obj_sphere._modelMat = center;
    obj_sphere._shader = &testNormalShader;
    testScene.AddObj( obj_sphere );


    CObj obj_spider( GEO_SPIDER );
    obj_spider._modelMat = right;
    obj_spider._shader = &testNormalShader;
    testScene.AddObj( obj_spider );
    

    // init scenes
    ////////////////////////////////////////////////////////



    // twbar
    // Send the new window size to AntTweakBar
    _gui_init( g_winWidth, g_winHeight );
    TwBar *bar = TwNewBar( "bar" );
    TwDefine( " bar label='camera properties' " );
    TwDefine(" GLOBAL help='a simple demo for look at camera' ");



    TwAddVarCB( bar, "wire", TW_TYPE_BOOL32, _setWireModeCB, _getWireModeCB, 0,  " label='Wireframe' help='Toggle wireframe display mode.' ");
    // vec struct for gui, which is mapped to a glm::vec3
    // so we don't have to steal the DIR3F type
    TwStructMember _tw_vec3Members[] = {
        { "x", TW_TYPE_FLOAT, offsetof( glm::vec3, x ), " step=0.01 help='vec3[0]' " },
        { "y", TW_TYPE_FLOAT, offsetof( glm::vec3, y ), " step=0.01 help='vec3[1]' " },
        { "z", TW_TYPE_FLOAT, offsetof( glm::vec3, z ), " step=0.01 help='vec3[2]' " },
    };

    TwType _TW_TYPE_VEC3F = TwDefineStruct( "Position", _tw_vec3Members, 3, sizeof(glm::vec3), NULL, NULL );
    
    // camera  pos   
    TwAddVarCB( bar, "camPos", _TW_TYPE_VEC3F, _setCameraPosCB, _getCameraPosCB, ( void* )( &simpleCam ),  " label='camera position' opened=true help='camera position' ");
    

    // - Directly redirect GLFW mouse button events to AntTweakBar
    glfwSetMouseButtonCallback( window, ( GLFWmousebuttonfun )_gui_mouseButtonCallback );
    // - Directly redirect GLFW mouse position events to AntTweakBar
    glfwSetCursorPosCallback( window, ( GLFWcursorposfun )_gui_mouseMoveCallback );
    // - Directly redirect GLFW mouse wheel events to AntTweakBar
    glfwSetScrollCallback( window, ( GLFWscrollfun )_gui_mouseScrollCallback );
    // - Directly redirect GLFW key events to AntTweakBar
    glfwSetKeyCallback( window, ( GLFWkeyfun )_gui_keyCallback );
    // - Directly redirect GLFW char events to AntTweakBar
    glfwSetCharCallback( window, ( GLFWcharfun )_gui_charCallback );


    // _printSPInfo( sp );

    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );

    // update and draw!
    while ( !glfwWindowShouldClose( window ) ) {
        _updateFPSCounter( window );

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glViewport( 0, 0, g_winWidth, g_winHeight );


        testScene.DrawScene();

        _gui_draw();

        glfwSwapBuffers( window );

        // events
        glfwPollEvents();
        if( GLFW_PRESS == glfwGetKey( window, GLFW_KEY_ESCAPE ) ) { 
            glfwSetWindowShouldClose( window, 1 ); 
        }
        if( GLFW_PRESS == glfwGetKey( window, GLFW_KEY_F11 ) ) {
            _screenPrint();
        }

    }

    // Terminate AntTweakBar and GLFW
    _gui_deinit();
    glfwTerminate();

    geos.Deinit();

    return 1;
}