#include "Light.h"


CLight::CLight( const vec3& t_pos, const CColor& t_ls, const CColor& t_ld, const CColor& t_la ) : _pos( t_pos ), _ls( t_ls ), _ld( t_ld ), _la( t_la )
{
}


CLight::~CLight(void)
{
}
