#include "math.hpp"


glm::vec4
sven::vec4_rotate_xy( const glm::vec4 &v, float theta )
{
    glm::vec4 u = v;

    u.x = v.x*cos(theta) - v.y*sin(theta);
    u.y = v.x*sin(theta) + v.y*cos(theta);

    return u;
}


void
sven::vec3_mat4_mul( float *v, float *M )
{
    float x = v[0]*M[0] + v[0]*M[1] + v[0]*M[2];  // + 0*M[3];
    float y = v[1]*M[4] + v[1]*M[5] + v[1]*M[6];  // + 0*M[7];
    float z = v[2]*M[8] + v[2]*M[9] + v[2]*M[10]; // + 0*M[11];

    v[0] = x;
    v[1] = y;
    v[2] = z;
}