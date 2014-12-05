#pragma once

#include <vector>
using std::vector;

class CShader
{
public:
    CShader( const string& t_vs, const string& t_fs );
    CShader( const string& t_vs, const string& t_gs, const string& t_ts, const string& t_fs );
    virtual ~CShader();

protected:
    string _vs, _fs, _ts, _gs;
    GLuint _sp;

protected:
    void 

};

