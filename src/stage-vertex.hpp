#pragma once

#include "vertex.hpp"
#include "texture.hpp"


namespace sven
{
    struct VS_in
    {
        glm::vec3 pos;
        glm::vec3 norm;
        glm::vec2 uv;
    };

    struct VS_out
    {
        glm::vec3 pos;
        glm::vec3 norm;
        glm::vec2 uv;
    };

    struct VS_uniforms
    {
        glm::mat4 P, V, M;
    };

}


