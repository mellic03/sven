#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>


#ifndef GLM_ENABLE_EXPERIMENTAL
    #define GLM_ENABLE_EXPERIMENTAL
#endif

#ifndef GLM_FORCE_SWIZZLE
    #define GLM_FORCE_SWIZZLE
#endif


#include <glm/glm.hpp>
#include <glm/gtx/common.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glm/detail/setup.hpp"
#include "glm/detail/_swizzle.hpp"
#include "glm/detail/_swizzle_func.hpp"




namespace sven
{
    struct Vertex
    {
        glm::vec4 pos;
        glm::vec3 norm;
        glm::vec2 uv;
    };

    struct VertexArray
    {
        float radiusSq = 0.0f;

        std::vector<glm::vec4> pos;
        std::vector<glm::vec3> wpos;
        std::vector<glm::vec3> norm;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> color;

        VertexArray() {  }

        VertexArray( uint32_t size )
        :   pos(size),
            wpos(size),
            norm(size),
            uv(size),
            color(size, glm::vec3(0, 1, 1))
        {

        }

        static void vertcpy( VertexArray &dst, const VertexArray &src,
                             int dst_idx, int src_idx, int nverts=3 )
        {
            std::memcpy(&dst.pos[dst_idx],   &src.pos[src_idx],   nverts*sizeof(glm::vec4));
            std::memcpy(&dst.wpos[dst_idx],  &src.wpos[src_idx],  nverts*sizeof(glm::vec3));
            std::memcpy(&dst.norm[dst_idx],  &src.norm[src_idx],  nverts*sizeof(glm::vec3));
            std::memcpy(&dst.uv[dst_idx],    &src.uv[src_idx],    nverts*sizeof(glm::vec2));
            std::memcpy(&dst.color[dst_idx], &src.color[src_idx], nverts*sizeof(glm::vec2));
        }

        void updateRadiusSq()
        {
            radiusSq = 0.0f;
            for (const glm::vec4 &p: pos)
            {
                radiusSq = std::max(radiusSq, glm::length2(glm::vec3(p)));
            }
        }

        uint32_t size() const
        {
            return pos.size();
        }

        void resize( uint32_t n )
        {
            pos.resize(n);
            wpos.resize(n);
            norm.resize(n);
            uv.resize(n);
            color.resize(n, glm::vec3(0, 1, 1));
        }

        void clear()
        {
            pos.clear();
            wpos.clear();
            norm.clear();
            uv.clear();
            color.clear();
        }

        void push_n( uint32_t n )
        {
            resize(size() + n);
        }
    };

    struct VaryingArray: public VertexArray
    {

    };
}
