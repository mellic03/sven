#include <iostream>
#include "window.hpp"
#include "rasterize.hpp"
#include "context.hpp"
#include "geometry.hpp"


#define WIN_SCALE 1
#define WIN_W (1024 / WIN_SCALE)
#define WIN_H (720 / WIN_SCALE)

#define PLANE_NEAR (0.1f)
#define PLANE_FAR  (64.0f)


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
                cam.pitch += 0.0025f * e.motion.yrel;
                cam.yaw   += 0.0025f * e.motion.xrel;
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



int WinMain()
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

    auto cube_buf0 = sven::gen_isosphere(0);
    auto cube_buf1 = sven::gen_isosphere(1);
    auto cube_buf2 = sven::gen_isosphere(2);
    auto cube_buf3 = sven::gen_isosphere(3);
    auto cube_buf4 = sven::gen_isosphere(4);

    bool running = true;
    float theta = 0.0f;

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
        // sven::quad(mx, my, 256, 256, theta, sven::packRGB(50, 255, 150));
        theta += 0.001f;


        glm::quat QPitch = glm::angleAxis(cam.pitch, glm::vec3(1, 0, 0));
        glm::quat QYaw   = glm::angleAxis(cam.yaw, glm::vec3(0, 1, 0));

        cam.V = glm::lookAt(
            cam.pos,
            cam.pos + glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        cam.V = glm::mat4_cast(QPitch * QYaw) * cam.V;


        glm::mat4 T0 = glm::translate(glm::mat4(1.0f), glm::vec3(-4, 0, -2));
        glm::mat4 T1 = glm::translate(glm::mat4(1.0f), glm::vec3(-2, 0, -2));
        glm::mat4 T2 = glm::translate(glm::mat4(1.0f), glm::vec3( 0, 0, -2));
        glm::mat4 T3 = glm::translate(glm::mat4(1.0f), glm::vec3(+2, 0, -2));
        glm::mat4 T4 = glm::translate(glm::mat4(1.0f), glm::vec3(+4, 0, -2));
        ctx.drawArrays(cube_buf0, T0, cam.P, cam.V);
        ctx.drawArrays(cube_buf1, T1, cam.P, cam.V);
        ctx.drawArrays(cube_buf2, T2, cam.P, cam.V);
        ctx.drawArrays(cube_buf3, T3, cam.P, cam.V);
        ctx.drawArrays(cube_buf4, T4, cam.P, cam.V);



        ctx.swapWindow(win, win_surface);
    }


    return 0;
}
