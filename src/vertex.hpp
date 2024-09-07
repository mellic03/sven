#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>


#ifndef GLM_ENABLE_EXPERIMENTAL
    #define GLM_ENABLE_EXPERIMENTAL
#endif

// #ifndef GLM_SWIZZLE
//     #define GLM_SWIZZLE
// #endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "glm/detail/setup.hpp"
// #include "glm/detail/_swizzle.hpp"
// #include "glm/detail/_swizzle_func.hpp"
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>




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
        std::vector<glm::vec4> pos;
        std::vector<glm::vec3> norm;
        std::vector<glm::vec2> uv;

        VertexArray() {  }

        VertexArray( uint32_t size )
        :   pos(size),
            norm(size),
            uv(size)
        {

        }

        size_t size()
        {
            return pos.size();
        }

        void clear()
        {
            pos.clear();
            norm.clear();
            uv.clear();
        }
    };

    struct VaryingArray
    {
        std::vector<glm::vec4> pos;
        std::vector<glm::vec3> norm;
        std::vector<glm::vec2> uv;
    };
}
