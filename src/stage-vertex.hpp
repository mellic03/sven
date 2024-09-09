#pragma once

#include "vertex.hpp"
#include "texture.hpp"


namespace sven
{
    struct VS_queue
    {
        RenderTexture target;
        std::vector<VertexArray> bins;
    };

    struct VS_in
    {
        glm::vec3 pos;
        glm::vec3 norm;
        glm::vec3 uv;
    };

    void renderstage_vertex( VS_queue &VS );

    glm::vec4 shader_vertex();
}


