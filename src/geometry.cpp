#include "geometry.hpp"
#include <iostream>


void computeNormal( sven::Vertex *buf )
{
    glm::vec3 A, B, C, N;

    A = buf[0].pos;
    B = buf[1].pos;
    C = buf[2].pos;
    N = glm::normalize(glm::cross(B-A, C-A));

    buf[0].norm = N;
    buf[1].norm = N;
    buf[2].norm = N;
}


void
sven::tri_subdivide( const Vertex *src, std::vector<Vertex> &dst )
{
    glm::vec3 A = src[0].pos;
    glm::vec3 B = src[1].pos;
    glm::vec3 C = src[2].pos;

    glm::vec3 AB = 0.5f * (A + B);
    glm::vec3 BC = 0.5f * (B + C);
    glm::vec3 CA = 0.5f * (C + A);

    static Vertex temp[12];

    for (int i=0; i<12; i++)
    {
        temp[i].norm = src[1].norm;
    }

    temp[0].pos = glm::vec4(A,  1.0f);
    temp[1].pos = glm::vec4(AB, 1.0f);
    temp[2].pos = glm::vec4(CA, 1.0f);

    temp[3].pos = glm::vec4(AB, 1.0f);
    temp[4].pos = glm::vec4(B,  1.0f);
    temp[5].pos = glm::vec4(BC, 1.0f);

    temp[6].pos = glm::vec4(AB, 1.0f);
    temp[7].pos = glm::vec4(BC, 1.0f);
    temp[8].pos = glm::vec4(CA, 1.0f);

    temp[9].pos  = glm::vec4(CA, 1.0f);
    temp[10].pos = glm::vec4(BC, 1.0f);
    temp[11].pos = glm::vec4(C,  1.0f);

    for (int i=0; i<12; i++)
    {
        dst.push_back(temp[i]);
    }
}





static void
copy_face( sven::Vertex *src, sven::Vertex *dst, float r, glm::vec3 axis )
{
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(r), axis);

    for (int i=0; i<6; i++)
    {
        dst[i].pos  = R * src[i].pos;
        dst[i].norm = glm::mat3(R) * src[i].norm;
    }
}


static void
copy_face2( sven::VertexArray &buf, int idx, float r, glm::vec3 axis )
{
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(r), axis);

    for (int i=0; i<6; i++)
    {
        buf.pos[idx+i]  = R * buf.pos[i];
        buf.norm[idx+i] = glm::mat3(R) * buf.norm[i];
    }
}



std::vector<sven::Vertex>
sven::gen_cube()
{
    float hw = 0.5f;
    float hh = 0.5f;

    std::vector<Vertex> buf(6*6);

    buf[0].pos = glm::vec4(+hw, 0.5f, +hh, 1.0f);
    buf[1].pos = glm::vec4(+hw, 0.5f, -hh, 1.0f);
    buf[2].pos = glm::vec4(-hw, 0.5f, -hh, 1.0f);

    buf[3].pos = glm::vec4(-hw, 0.5f, -hh, 1.0f);
    buf[4].pos = glm::vec4(-hw, 0.5f, +hh, 1.0f);
    buf[5].pos = glm::vec4(+hw, 0.5f, +hh, 1.0f);

    copy_face(&(buf[0]), &(buf[1*6]), +90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    copy_face(&(buf[0]), &(buf[2*6]), -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    copy_face(&(buf[0]), &(buf[3*6]), 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    copy_face(&(buf[0]), &(buf[4*6]), -90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    copy_face(&(buf[0]), &(buf[5*6]), +90.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    return buf;
}



static sven::VertexArray gen_cube2()
{
    using namespace sven;
    
    float hw = 0.5f;
    float hh = 0.5f;

    VertexArray buf(6*6);

    buf.pos[0] = glm::vec4(+hw, 0.5f, +hh, 1.0f);
    buf.pos[1] = glm::vec4(+hw, 0.5f, -hh, 1.0f);
    buf.pos[2] = glm::vec4(-hw, 0.5f, -hh, 1.0f);

    buf.pos[3] = glm::vec4(-hw, 0.5f, -hh, 1.0f);
    buf.pos[4] = glm::vec4(-hw, 0.5f, +hh, 1.0f);
    buf.pos[5] = glm::vec4(+hw, 0.5f, +hh, 1.0f);

    copy_face2(buf, 1*6, +90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    copy_face2(buf, 2*6, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    copy_face2(buf, 3*6, 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    copy_face2(buf, 4*6, -90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    copy_face2(buf, 5*6, +90.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    return buf;
}




std::vector<sven::Vertex>
sven::gen_isosphere( uint32_t subdivisions )
{
    std::vector<Vertex> buf0 = gen_cube();
    std::vector<Vertex> buf1;

    auto *b0 = &buf0;
    auto *b1 = &buf1;

    for (uint32_t i=0; i<subdivisions; i++)
    {
        auto &A = *b0;
        auto &B = *b1;

        B.clear();

        for (int j=0; j<A.size(); j+=3)
        {
            tri_subdivide(&A[j], B);
        }

        std::swap(b0, b1);
    }

    auto &A = *b0;
    std::cout << "A.size(): " << A.size() << "\n";

    for (int i=0; i<A.size(); i++)
    {
        A[i].pos = glm::vec4(glm::normalize(glm::vec3(A[i].pos)), 1.0f);
    }

    for (int i=0; i<A.size(); i+=3)
    {
        computeNormal(&A[i]);
    }

    return A;
}




sven::VertexArray
sven::gen_isosphere2( uint32_t subdivisions )
{
    VertexArray buf0 = gen_cube2();
    VertexArray buf1;

    auto *b0 = &buf0;
    auto *b1 = &buf1;

    for (uint32_t i=0; i<subdivisions; i++)
    {
        auto &A = *b0;
        auto &B = *b1;

        B.clear();

        for (int j=0; j<A.size(); j+=3)
        {
            tri_subdivide(&A[j], B);
        }

        std::swap(b0, b1);
    }

    auto &A = *b0;
    std::cout << "A.size(): " << A.size() << "\n";

    for (int i=0; i<A.size(); i++)
    {
        A[i].pos = glm::vec4(glm::normalize(glm::vec3(A[i].pos)), 1.0f);
    }

    for (int i=0; i<A.size(); i+=3)
    {
        computeNormal(&A[i]);
    }

    return A;
}

