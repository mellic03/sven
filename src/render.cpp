#include "render.hpp"
#include "context.hpp"


void*
sven::vbuffer_new( uint32_t stride, uint32_t count )
{
    uint32_t nbytes = sizeof(float) * stride * count;
    return std::malloc(nbytes);
}


void
sven::drawArrays( const VertexBuffer &buf )
{
    auto &ctx    = *sven::getCurrentContext();
    auto &binner = ctx.binner();

    int res = binner.insert(buf);

    if (res == -1)
    {
        // ruh roh
    } 

}



// void
// sven::context::vertex_stage( const VertexArray &src, VaryingArray &dst, const RenderState &RS )
// {
//     static VertexArray *tmp[2] = { new VertexArray, new VertexArray };

//     auto &A = *tmp[0];
//     auto &B = *tmp[1];

//     B.clear();
//     for (uint32_t i=0; i<src.size(); i+=3)
//     {
//         vertex_backface(src, i, B, RS);
//     }

//     for (uint32_t i=0; i<B.size(); i+=3)
//     {
//         varying_stage(B, dst, i, RS);
//     }
// }


// void
// sven::context::vertex_backface( const VertexArray &src, int src_idx, VertexArray &dst,
//                                 const RenderState &RS )
// {
//     glm::vec3 A = src.pos[src_idx+0];
//     glm::vec3 B = src.pos[src_idx+1];
//     glm::vec3 C = src.pos[src_idx+2];
    

//     glm::vec3 world = RS.T * glm::vec4(A, 1.0f);
//     glm::vec3 N = glm::normalize(glm::cross(B-A, C-A));
//               N = RS.T * glm::vec4(N, 0.0f);

//     glm::vec3 viewdir = glm::normalize(RS.viewpos - world);

//     if (glm::dot(N, viewdir) < 0.0f)
//     {
//         return;
//     }

//     int dst_idx = dst.size();
//     dst.push_n(3);
//     VertexArray::vertcpy(dst, src, dst_idx, src_idx, 3);
// }





void
sven::drawElements( const VertexBuffer &buf, const std::vector<uint32_t> &indices )
{

}



