#include "Scene.h"


CScene::CScene()
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
        float dist = _objects[ i ].RayIntersectTest( t_ray );
        if( dist < 0 ) continue;
        if( dist < rayObjDist ) {
            rayObjDist = dist;
            index = i;
        }
    }

    return index;
}