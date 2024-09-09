#include <iostream>
#include "window.hpp"
#include "rasterize.hpp"
#include "context.hpp"
#include "geometry.hpp"


#define WIN_SCALE 2
#define WIN_W ((64*24) / WIN_SCALE)
#define WIN_H ((64*18) / WIN_SCALE)

#define PLANE_NEAR (1.0f)
#define PLANE_FAR  (100.0f)


int mx, my;

struct sven_Camera
{
    glm::vec3 pos   = glm::vec3(0.0f);
    float     pitch = 0.0f;
    float     roll  = 0.0f;
    float     yaw   = 0.0f;

    glm::mat4 P, V;

    sven_Camera()
    {
        P = glm::perspective(
            glm::radians(90.0f),
            float(WIN_W) / float(WIN_H),
            PLANE_NEAR,
            PLANE_FAR
        );
    }

    void updateView()
    {
        const float PI = float(M_PI);
        // pitch = glm::mod(pitch+PI, 2.0f*PI) - PI;
        // yaw   = glm::mod(yaw+PI,   2.0f*PI) - PI;

        glm::mat3 Ryaw    = glm::mat3(glm::rotate(glm::mat4(1), yaw, glm::vec3(0, 1, 0)));
        glm::mat3 Rpitch  = glm::mat3(glm::rotate(glm::mat4(1), pitch, glm::vec3(1, 0, 0)));
        glm::vec3 viewdir = glm::normalize(Ryaw * Rpitch * glm::vec3(0, 0, -1));

        V = glm::lookAt(pos, pos+0.01f*viewdir, glm::vec3(0, 1, 0));
    }

    glm::vec3 getUp()
    {
        return glm::inverse(glm::mat3(V)) * glm::vec3(0.0f, 1.0f, 0.0f);
    }

    glm::vec3 getRight()
    {
        return glm::inverse(glm::mat3(V)) * glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::vec3 getFront()
    {
        return glm::inverse(glm::mat3(V)) * glm::vec3(0.0f, 0.0f, -1.0f);
    }

};

sven_Camera cam;


void on_events( float dt, SDL_Event e );

void poll_events( float dt, bool *running )
{
    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        if ((e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) ||
            (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) 
        {
            *running = false;
        }

        on_events(dt, e);
    }
}


void on_events( float dt, SDL_Event e )
{
    switch (e.type)
    {
        case SDL_MOUSEBUTTONDOWN:
            SDL_SetRelativeMouseMode((SDL_bool)(!SDL_GetRelativeMouseMode()));
        break;
        
        case SDL_MOUSEMOTION:
            if (SDL_GetRelativeMouseMode() == SDL_TRUE)
            {
                cam.pitch -= 0.0025f * e.motion.yrel;
                cam.yaw   -= 0.0025f * e.motion.xrel;
            }
        break;

        default:
            break;
    }
}


void update( float dt )
{
    const uint8_t *state = SDL_GetKeyboardState(NULL);

    glm::vec3 up    = cam.getUp();
    glm::vec3 right = cam.getRight();
    glm::vec3 front = cam.getFront();
    glm::vec3 delta = glm::vec3(0.0f);

    if (state[SDL_SCANCODE_A]) delta -= dt * right;
    if (state[SDL_SCANCODE_D]) delta += dt * right;
    if (state[SDL_SCANCODE_S]) delta -= dt * front;
    if (state[SDL_SCANCODE_W]) delta += dt * front;

    if (fabs(delta.x) > 0.00025f || fabs(delta.z) > 0.00025f)
    {
        delta = glm::length(delta) * glm::normalize(delta * glm::vec3(1, 0, 1));
    }

    if (state[SDL_SCANCODE_SPACE]) delta.y += dt;
    if (state[SDL_SCANCODE_LCTRL]) delta.y -= dt;

    if (state[SDL_SCANCODE_UP])    cam.pitch -= dt * 1.0f;
    if (state[SDL_SCANCODE_DOWN])  cam.pitch += dt * 1.0f;
    if (state[SDL_SCANCODE_LEFT])  cam.yaw -= dt * 1.0f;
    if (state[SDL_SCANCODE_RIGHT]) cam.yaw += dt * 1.0f;

    cam.pos += delta;
}



int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win;
    SDL_Surface *win_surface;

    win = SDL_CreateWindow(
        "Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_W * WIN_SCALE,
        WIN_H * WIN_SCALE,
        0
    );

    win_surface = SDL_GetWindowSurface(win);




    sven::context ctx(WIN_W, WIN_H);
    ctx.cullFace(false);

    sven::Texture crate_tex = sven::texture_load("crate.png", true);
    sven::Texture grid_tex  = sven::texture_load("grid.png",  true);
    sven::Texture grass_tex = sven::texture_load("grass.png", true);

    sven::VertexArray quad_buffer = sven::gen_quad(8.0f, 8.0f, 8.0f, 8.0f, 3);

    std::vector<sven::VertexArray> buffers;
    float theta[3]  = { 0.0f, 0.0f, 0.0f };
    float dtheta[3] = { (rand()%100-50)/100.0f, (rand()%100-50)/100.0f, (rand()%100-50)/100.0f };
    for (int i=0; i<3; i++)
    {
        buffers.push_back(sven::gen_isosphere(i));
    }





    bool running = true;

    uint64_t prev_ticks = SDL_GetTicks64();
    int count = 0;
    float avg = 0.0f;

    while (running)
    {
        uint64_t curr_ticks = SDL_GetTicks64();
        uint64_t delta = curr_ticks - prev_ticks;
        prev_ticks = curr_ticks;

        float dt = delta / 1000.0f;

        count += 1;
        avg = glm::mix(avg, dt, 127.0f / 128.0f);

        if (count % 128 == 0)
        {
            std::cout << "Avg dt: " << avg << "\n";
            count = 1;
        }

        ctx.clear(sven::COLOR_BUFFER_BIT | sven::DEPTH_BUFFER_BIT);

        poll_events(dt, &running);
        SDL_PumpEvents();
        SDL_GetMouseState(&mx, &my);

        update(dt);
        cam.updateView();

        for (int i=0; i<buffers.size(); i++)
        {
            theta[i] += dt * dtheta[i];

            glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(-2 + 2*i, 0, -2));
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), theta[i], glm::vec3(0, 1, 0));

            ctx.bindTexture(crate_tex);
            ctx.drawArrays(buffers[i], T*R, cam.P, cam.V);
        }


        glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0, -2, 0));
        ctx.bindTexture(grass_tex);
        ctx.drawArrays(quad_buffer, T, cam.P, cam.V);

        ctx.swapWindow(win, win_surface);
    }


    return 0;
}
