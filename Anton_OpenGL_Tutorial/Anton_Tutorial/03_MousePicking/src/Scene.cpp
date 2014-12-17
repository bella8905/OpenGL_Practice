#include "Scene.h"


CScene::CScene()
{
}


CScene::~CScene()
{

}


void CScene::DrawScene() {
    for( us i = 0 ; i < _objects.size(); ++i ) {
        _objects[i].DrawObj( false );
    }
}

void CScene::AddObj( CObj& t_obj ) {
    _objects.push_back( t_obj );
}