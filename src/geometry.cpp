#include "geometry.hpp"
#include <iostream>


void computeNormal( sven::VertexArray &buf, int idx )
{
    glm::vec3 A, B, C, N;

    A = buf.pos[idx+0];
    B = buf.pos[idx+1];
    C = buf.pos[idx+2];
    N = glm::normalize(glm::cross(B-A, C-A));

    buf.norm[idx+0] = N;
    buf.norm[idx+1] = N;
    buf.norm[idx+2] = N;
}


void
sven::tri_subdivide( const sven::VertexArray &src, int vidx, sven::VertexArray &dst )
{
    glm::vec3 A = src.pos[vidx+0];
    glm::vec3 B = src.pos[vidx+1];
    glm::vec3 C = src.pos[vidx+2];

    glm::vec3 AB = 0.5f * (A + B);
    glm::vec3 BC = 0.5f * (B + C);
    glm::vec3 CA = 0.5f * (C + A);

    glm::vec3 positions[12] = {
        A, AB, CA, AB, B, BC, AB, BC, CA, CA, BC, C
    };


    glm::vec2 uv_A = src.uv[vidx+0];
    glm::vec2 uv_B = src.uv[vidx+1];
    glm::vec2 uv_C = src.uv[vidx+2];

    glm::vec2 uv_AB = 0.5f * (uv_A + uv_B);
    glm::vec2 uv_BC = 0.5f * (uv_B + uv_C);
    glm::vec2 uv_CA = 0.5f * (uv_C + uv_A);

    glm::vec2 texcoords[12] = {
        uv_A, uv_AB, uv_CA, uv_AB, uv_B, uv_BC, uv_AB, uv_BC, uv_CA, uv_CA, uv_BC, uv_C
    };

    int idx = dst.size();
    dst.push_n(12);

    for (int i=0; i<12; i++)
    {
        dst.pos[idx+i]  = glm::vec4(positions[i], 1.0f);
        dst.norm[idx+i] = src.norm[vidx+i];
        dst.uv[idx+i]   = texcoords[i];
    }
}




static void
copy_face( sven::VertexArray &buf, int idx, float r, glm::vec3 axis )
{
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(r), axis);

    for (int i=0; i<6; i++)
    {
        buf.pos[idx+i]  = R * buf.pos[i];
        buf.norm[idx+i] = glm::mat3(R) * buf.norm[i];
        buf.uv[idx+i]   = buf.uv[i];
    }
}


void
sven::gen_quad( sven::VertexArray &dst, int idx )
{
    static constexpr float hw = 0.5f;
    static constexpr float hh = 0.5f;

    dst.pos[idx+0] = glm::vec4(+hw, 0.5f, +hh, 1.0f);
    dst.pos[idx+1] = glm::vec4(+hw, 0.5f, -hh, 1.0f);
    dst.pos[idx+2] = glm::vec4(-hw, 0.5f, -hh, 1.0f);

    dst.uv[idx+0] = glm::vec2(1.0f, 1.0f);
    dst.uv[idx+1] = glm::vec2(1.0f, 0.0f);
    dst.uv[idx+2] = glm::vec2(0.0f, 0.0f);


    dst.pos[idx+3] = glm::vec4(-hw, 0.5f, -hh, 1.0f);
    dst.pos[idx+4] = glm::vec4(-hw, 0.5f, +hh, 1.0f);
    dst.pos[idx+5] = glm::vec4(+hw, 0.5f, +hh, 1.0f);

    dst.uv[idx+3] = glm::vec2(0.0f, 0.0f);
    dst.uv[idx+4] = glm::vec2(0.0f, 1.0f);
    dst.uv[idx+5] = glm::vec2(1.0f, 1.0f);
}


sven::VertexArray
sven::gen_quad( float xscale, float zscale, float uscale, float vscale, int subdivisions )
{
    static VertexArray *buffers[2] = { new VertexArray(6), new VertexArray(6) };

    gen_quad(*buffers[0], 0);

    for (int i=0; i<6; i++)
    {
        buffers[0]->pos[i].x *= xscale;
        buffers[0]->pos[i].z *= zscale;
    
        buffers[0]->uv[i].x *= uscale;
        buffers[0]->uv[i].y *= vscale;
    }


    for (int i=0; i<subdivisions; i++)
    {
        auto &A = *buffers[0];
        auto &B = *buffers[1];

        B.clear();

        for (int j=0; j<A.size(); j+=3)
        {
            tri_subdivide(A, j, B);
        }

        std::swap(buffers[0], buffers[1]);
    }

    auto &A = *buffers[0];
    A.updateRadiusSq();

    return A;
}





sven::VertexArray
sven::gen_cube()
{
    using namespace sven;
    
    float hw = 0.5f;
    float hh = 0.5f;

    VertexArray buf(6*6);

    sven::gen_quad(buf, 0);

    // buf.pos[0] = glm::vec4(+hw, 0.5f, +hh, 1.0f);
    // buf.pos[1] = glm::vec4(+hw, 0.5f, -hh, 1.0f);
    // buf.pos[2] = glm::vec4(-hw, 0.5f, -hh, 1.0f);

    // buf.pos[3] = glm::vec4(-hw, 0.5f, -hh, 1.0f);
    // buf.pos[4] = glm::vec4(-hw, 0.5f, +hh, 1.0f);
    // buf.pos[5] = glm::vec4(+hw, 0.5f, +hh, 1.0f);

    copy_face(buf, 1*6, +90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    copy_face(buf, 2*6, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    copy_face(buf, 3*6, 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    copy_face(buf, 4*6, -90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    copy_face(buf, 5*6, +90.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    buf.updateRadiusSq();
    return buf;
}


sven::VertexArray
sven::gen_isosphere( uint32_t subdivisions )
{
    static VertexArray *buffers[2] = { new VertexArray, new VertexArray };

    *buffers[0] = gen_cube();

    for (uint32_t i=0; i<subdivisions; i++)
    {
        auto &A = *buffers[0];
        auto &B = *buffers[1];

        B.clear();

        for (int j=0; j<A.size(); j+=3)
        {
            tri_subdivide(A, j, B);
        }

        std::swap(buffers[0], buffers[1]);
    }

    auto &A = *buffers[0];
    std::cout << "A.size(): " << A.size() << "\n";

    for (int i=0; i<A.size(); i++)
    {
        A.pos[i] = 0.5f * glm::vec4(glm::normalize(glm::vec3(A.pos[i])), 1.0f);
    }

    for (int i=0; i<A.size(); i+=3)
    {
        computeNormal(A, i);
    }

    A.updateRadiusSq();

    return A;
}


