#include "Scene.h"


CScene::CScene() : _selectedObjIdx( -1 )
{
}


CScene::~CScene()
{

}


void CScene::DrawScene() {
    for( us i = 0 ; i < _objects.size(); ++i ) {
        _objects[i].DrawObj();
    }
}

void CScene::AddObj( CObj& t_obj ) {
    _objects.push_back( t_obj );
}

int CScene::GetRayHitObjIdx( const Utl::SRay& t_ray ) {
    int index = -1;
    float rayObjDist = std::numeric_limits<float>::infinity();
    for( us i = 0 ; i < _objects.size(); ++i ) {
        float dist = _objects[ i ].RayIntersectTestWithBB( t_ray );
        if( dist < 0 ) continue;
        if( dist < rayObjDist ) {
            rayObjDist = dist;
            index = i;
        }
    }

    return index;
}

void CScene::UpdateScene() {
    // update selected obj
    for( int i = 0; i < ( int )_objects.size(); ++i ) {
        if( i == _selectedObjIdx ) {
            _objects[ i ]._selected = true;
        }
        else {
            _objects[ i ]._selected = false;
        }
    }
}

void CScene::StopRotObj( const int& t_objIdx ) {
    if(  t_objIdx >= 0 && t_objIdx < (int)_objects.size() ) {
        _objects[ t_objIdx ].EndRot();
    }
}