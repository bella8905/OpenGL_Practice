/////////////////////////////////////////////////////////////////
//
//  Practice_11_19_14 - main
//
//  Implementation of Anton's OpenGL tutorial
//  p3 - 
//  obj / scene class. geo / sd container
//  bb / raybased mouse picking
//  arcball rotation
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
#include "glm/gtx/transform.hpp"


#include <GL/AntTweakBar.h>

#include <iostream>
#include <fstream>
#include <streambuf>

using namespace std;

int g_winWidth = 640;
int g_winHeight = 480;

bool g_drawWireModel = false;
CScene g_scene;

const string g_imageFilePrefix = "images/screenshot_"; 
bool g_mousePressed = false;

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

Utl::SRay _getRayFromMouse( const float& t_posx, const float& t_posy ) {
    // nds ( -1 , 1 )
    float x = ( 2.f * t_posx ) / g_winWidth - 1.f;
    float y = 1.f - ( 2.f * t_posy ) / g_winHeight;
    vec3 dir_nds( x, y, 1.f );
    vec4 dir_clip( dir_nds.x, dir_nds.y, -1.f, 0.f );
    // get the current used camera's proj matrix
    vec4 dir_eye = glm::inverse( g_simpleCam.GetProjMat() ) * dir_clip;
    dir_eye = vec4( dir_eye.x, dir_eye.y, -1.f, 0.f );
    Utl::SRay ray_eye( vec4( 0.f, 0.f, 0.f, 1.f ), dir_eye );
    Utl::SRay ray_wor = ray_eye.Transform( glm::inverse( g_simpleCam.GetViewMat() ) );

    return ray_wor;
}



void _gui_mouseButtonCallback( GLFWwindow* t_window, int t_btn, int t_action, int t_mods ) {
    // right mouse click to rotate
    if( t_action == GLFW_PRESS && t_btn == GLFW_MOUSE_BUTTON_RIGHT ){
        cout<<"mouse pressed"<<endl;
        g_mousePressed = true;

        double xpos, ypos;
        glfwGetCursorPos( t_window, &xpos, &ypos );
        Utl::SRay ray_wor = _getRayFromMouse( (float)xpos, (float)ypos );
        g_scene.RayIntersectWithArcball( ray_wor, true );

    } 
    if( t_action == GLFW_RELEASE && t_btn == GLFW_MOUSE_BUTTON_RIGHT ) {
        cout<<"mouse released"<<endl;
        g_mousePressed = false;
        g_scene.StopRotObj( g_scene._selectedObjIdx );
    } 

    // do selection test
    // left mouse click to select
    if( t_action == GLFW_PRESS && t_btn == GLFW_MOUSE_BUTTON_LEFT ) {
        double xpos, ypos;
        glfwGetCursorPos( t_window, &xpos, &ypos );
        Utl::SRay ray_wor = _getRayFromMouse( (float)xpos, (float)ypos );

        g_scene._selectedObjIdx = g_scene.GetRayHitObjIdx( ray_wor );
        g_scene.UpdateScene();
        
        // if we select a different obj, stop rot obj
    }

    // middle btn to revert model matrix
    if( t_action == GLFW_PRESS && t_btn == GLFW_MOUSE_BUTTON_MIDDLE ) {
        g_scene.RevertSelectedObj();
    }

    _gui_onMouseClicked( t_btn, t_action );
}

void _gui_mouseMoveCallback( GLFWwindow* t_window, double t_x, double t_y ) {

    if( g_mousePressed )
    {
        cout<<"mouse held"<<endl;
        double xpos, ypos;
        glfwGetCursorPos( t_window, &xpos, &ypos );
        Utl::SRay ray_wor = _getRayFromMouse( (float)xpos, (float)ypos );
        g_scene.RayIntersectWithArcball( ray_wor, false );
    }




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
        { GL_LINE_WIDTH_RANGE,                  "GL_LINE_WIDTH_RANGE",                  VEC_INT },
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

// arcball 
void TW_CALL _setArcballCB(  const void* t_value, void* t_clientData ) {
    CObj::_drawAcball = *(bool*)t_value;
}

void TW_CALL _getArcballCB(  void* t_value, void* t_clientData ) {
    *(bool*)t_value = CObj::_drawAcball;
}

void TW_CALL _setArcballRadiusCB(  const void* t_value, void* t_clientData ) {
    // CObj::_arcball_radius = *(float*)t_value;
}

void TW_CALL _getArcballRadiusCB(  void* t_value, void* t_clientData ) {
    *(float*)t_value = 0.f;
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


// init and deint modules
void _initModules()
{
    CShaderContainer::GetInstance().Init();
    CGeoContainer::GetInstance().Init();
    SArcball::InitArcball();
}

void _deinitModules() {
    SArcball::DeinitArcball();
    CGeoContainer::GetInstance().Deinit();
    CShaderContainer::GetInstance().Deinit();

}

void update( GLFWwindow* t_window )
{
    _updateFPSCounter( t_window );


    bool isTest = false;
    if( isTest ) {
        static float angle_degree = 60;
        static vec3 axis_unnormailzed( 1.f, 1.f, 0.f );
        
        float angle = angle_degree * Utl::g_o2Pi;
        vec3 axis = glm::normalize( axis_unnormailzed );
        mat3 rot_matrix = Utl::ToMat3( glm::rotate( mat4(1.f), angle, axis ) );

        // use quaternion
        glm::quat rotQuat = glm::angleAxis( angle, axis );
        mat3 rot_quat = glm::toMat3( rotQuat ); 

        bool isTestMatrix = true;
        if( isTestMatrix ) {
            g_scene._objects[0].SetRot( rot_matrix );
        }
        else 
        {
            g_scene._objects[0].SetRot( rot_quat );
        }

        cout<<endl;
    }
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
    glEnable(GL_POLYGON_OFFSET_FILL);

    glPointSize( 3.f );

    _initModules();
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

    g_simpleCam.Setup( camPos, camFace, camUp, clipNear, clipFar, clipFOV, clipAspect );

    // light
    vec3 lightPos( 0.f, 0.f, 2.f );
    vec3 lightLs( 1.f, 1.f, 1.f );
    vec3 lightLd( 0.7f, 0.7f, 0.7f );
    vec3 lightLa( 0.2f, 0.2f, 0.2f );

    g_simpleLight.Setup( lightPos, lightLs, lightLd, lightLa );

    // material
    bool hasSpecular = true;
    Utl::CColor ks( 1.f, 1.f, 1.f );
    float specularExp = 10.f;
    Utl::CColor kd( 1.f, 0.5f, 0.f );
    Utl::CColor ka( 1.f, 1.f, 1.f );

    CMaterial blinnMat( kd, hasSpecular, ks, specularExp, ka );

    // geos
    vec3 translate_left( -0.8f, 0.f, 0.f );
    vec3 translate_center( 0.f, 0.f, 0.f );
    vec3 translate_right( 0.8f, 0.f, 0.f );
    glm::mat3 rot_noRot( 1.f );
    mat4 rot_x30 = glm::rotate(glm::mat4(), 30 * Utl::g_o2Pi, glm::vec3(1,0,0) );
    float scale_s = 0.5f;
    float scale_xs = 0.1f;

    // cube 
    CObj obj_cube( GEO_UNIT_CUBE );
    obj_cube.SetupModelMatrix( translate_left, rot_x30, scale_s );
    obj_cube._drawBB = true;
    obj_cube._shaderType = SD_NORMAL_TEST;
    g_scene.AddObj( obj_cube );

    CObj obj_sphere( GEO_UNIT_SPHERE  );
    // obj_sphere._material = blinnMat;
    obj_sphere.SetupModelMatrix( translate_right, rot_noRot, scale_xs );
    obj_sphere._shaderType = SD_PHONG;
    obj_sphere._drawBB = true;
    // g_scene.AddObj( obj_sphere );


    CObj obj_spider( GEO_TRIANGLE );
    obj_spider.SetupModelMatrix( translate_center, rot_noRot, scale_s );
    obj_spider._shaderType = SD_NORMAL_TEST;
    obj_spider._drawBB = true;
    // g_scene.AddObj( obj_spider );
    

    // init scenes
    ////////////////////////////////////////////////////////



    // twbar
    // Send the new window size to AntTweakBar
    _gui_init( g_winWidth, g_winHeight );
    TwBar *bar = TwNewBar( "bar" );
    TwDefine( " bar label='camera properties' " );
    TwDefine(" GLOBAL help='a simple demo for look at camera' ");



    TwAddVarCB( bar, "wire", TW_TYPE_BOOL32, _setWireModeCB, _getWireModeCB, 0,  " label='Wireframe' help='Toggle wireframe display mode.' ");
    TwAddVarCB( bar, "arcball", TW_TYPE_BOOL32, _setArcballCB, _getArcballCB, 0,  " label='Arcball' help='Toggle arcball on or off.' ");
    TwAddVarCB( bar, "arcball radius", TW_TYPE_FLOAT, _setArcballRadiusCB, _getArcballRadiusCB, 0,  " label='Arcball Radius' step=0.01 max=0.5 min=0.1 ");

    // vec struct for gui, which is mapped to a glm::vec3
    // so we don't have to steal the DIR3F type
    TwStructMember _tw_vec3Members[] = {
        { "x", TW_TYPE_FLOAT, offsetof( glm::vec3, x ), " step=0.01 help='vec3[0]' " },
        { "y", TW_TYPE_FLOAT, offsetof( glm::vec3, y ), " step=0.01 help='vec3[1]' " },
        { "z", TW_TYPE_FLOAT, offsetof( glm::vec3, z ), " step=0.01 help='vec3[2]' " },
    };

    TwType _TW_TYPE_VEC3F = TwDefineStruct( "Position", _tw_vec3Members, 3, sizeof(glm::vec3), NULL, NULL );
    
    // camera  pos   
    TwAddVarCB( bar, "camPos", _TW_TYPE_VEC3F, _setCameraPosCB, _getCameraPosCB, ( void* )( &g_simpleCam ),  " label='camera position' opened=true help='camera position' ");
    

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

        update( window ); 

        glClearColor( 0.5f, 0.5f, 0.5f, 1.f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glViewport( 0, 0, g_winWidth, g_winHeight );


        g_scene.DrawScene();

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

    _deinitModules();

    return 1;
}