#include <SDL2/SDL.h>
#include <Eigen/Dense>
#include <cmath>

using Eigen::MatrixXd;

#define WINDOW_WIDTH 800
#define ROTATION_ANGLE 0.001
#define ROTATION_ANGLE_2 0.0005
#define ROTATION_ANGLE_3 0.00075
#define EDGE_NUMBER 12
#define VERTEX_NUMBER 8
#define FOCAL_LENGTH 775.0

struct coords{
  float x;
  float y;
  float z;
};
struct vertex{
    coords real;
    coords projected;
};
struct edge{
    vertex* v1;
    vertex* v2;
};

void MySleep( Uint32 interval )
{
    struct Container
    {
        static Uint32 TimerCallback( Uint32 interval, void* param )
        {
            SDL_Event event;
            event.type = SDL_USEREVENT;
            event.user.code = 42;
            SDL_PushEvent( &event );
            return 0;
        }
    };

    SDL_AddTimer( interval, Container::TimerCallback, NULL );

    SDL_Event event;
    while( SDL_WaitEvent( &event ) )
    {
        if( event.type == SDL_USEREVENT && event.user.code == 42 )
            break;
    }
}

void project_vertex(vertex& v, float focal_length){
    v.projected.x = (focal_length*v.real.x)/(focal_length+v.real.z) + WINDOW_WIDTH/2;
    v.projected.y = (focal_length*v.real.y)/(focal_length+v.real.z) + WINDOW_WIDTH/2;
}

void rotate_vertex(vertex& v, const char rotation_axis, float rotation_angle){
    MatrixXd unrotated_vertex(1, 3);
    MatrixXd rotation_matrix(3, 3);
    MatrixXd rotated_vertex(1, 3);

    unrotated_vertex << v.real.x, v.real.y, v.real.z;

    switch(rotation_axis){
        case 'x':
            rotation_matrix << 1,         0,                    0,
                               0, cos(rotation_angle), -sin(rotation_angle),
                               0, sin(rotation_angle), cos(rotation_angle);
        break;
        case 'z':
            rotation_matrix << cos(rotation_angle), -sin(rotation_angle), 0,
                               sin(rotation_angle), cos(rotation_angle),  0,
                                        0,                  0,            1;
        break;
        default:
            rotation_matrix << cos(rotation_angle),    0,  sin(rotation_angle),
                                        0,             1,            0,
                               -sin(rotation_angle),   0,  cos(rotation_angle);
        break;
    }
    rotated_vertex = unrotated_vertex * rotation_matrix;
    v.real.x = rotated_vertex(0,0);
    v.real.y = rotated_vertex(0,1);
    v.real.z = rotated_vertex(0,2);
}

int main(void) {
    //SDL init things/////////////////////////////////////////////////////////////
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_WIDTH, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);//update renderer
    //creating DaCube/////////////////////////////////////////////////////////
    vertex ve[8];//vertex table

    ve[0].real.x = -100.0; ve[0].real.y = 100.0; ve[0].real.z = 100.0;//A
    ve[1].real.x = 100.0; ve[1].real.y = 100.0; ve[1].real.z = 100.0;//B
    ve[2].real.x = 100.0; ve[2].real.y = -100.0; ve[2].real.z = 100.0;//C
    ve[3].real.x = -100.0; ve[3].real.y = -100.0; ve[3].real.z = 100.0;//D
    
    ve[4].real.x = -100.0; ve[4].real.y = 100.0; ve[4].real.z = -100.0;//E
    ve[5].real.x = 100.0; ve[5].real.y = 100.0; ve[5].real.z = -100.0;//F
    ve[6].real.x = 100.0; ve[6].real.y = -100.0; ve[6].real.z = -100.0;//G
    ve[7].real.x = -100.0; ve[7].real.y = -100.0; ve[7].real.z = -100.0;//H


    edge ed[12];//edge table

    ed[0].v1 = &ve[0]; ed[0].v2 = &ve[1];//AB
    ed[1].v1 = &ve[1]; ed[1].v2 = &ve[2];//BC
    ed[2].v1 = &ve[2]; ed[2].v2 = &ve[3];//CD
    ed[3].v1 = &ve[3]; ed[3].v2 = &ve[0];//DA

    ed[4].v1 = &ve[4]; ed[4].v2 = &ve[5];//EF
    ed[5].v1 = &ve[5]; ed[5].v2 = &ve[6];//FG
    ed[6].v1 = &ve[6]; ed[6].v2 = &ve[7];//GH
    ed[7].v1 = &ve[7]; ed[7].v2 = &ve[4];//HE

    ed[8].v1 = &ve[0]; ed[8].v2 = &ve[4];//AE
    ed[9].v1 = &ve[1]; ed[9].v2 = &ve[5];//BF
    ed[10].v1 = &ve[2]; ed[10].v2 = &ve[6];//CG
    ed[11].v1 = &ve[3]; ed[11].v2 = &ve[7];//DH

    while (1) {
        //defining projected coords//////////////////////////////////////////////////
        for (int i = 0; i < VERTEX_NUMBER; ++i){
            project_vertex(ve[i], FOCAL_LENGTH);
        }
        //drawing edges between projected coords////////////////////////////////////
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//set renderer color to white
        for (int i = 0; i < EDGE_NUMBER; ++i)
        {
            SDL_RenderDrawLine(renderer, ed[i].v1->projected.x, ed[i].v1->projected.y, ed[i].v2->projected.x, ed[i].v2->projected.y);
        }
        SDL_RenderPresent(renderer);//update renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);//clear window

        //Rotating the cube arround Y axis
        
        for (int i = 0; i < VERTEX_NUMBER; ++i)
        {
            rotate_vertex(ve[i], 'y', ROTATION_ANGLE);
        }
        for (int i = 0; i < VERTEX_NUMBER; ++i)
        {
            rotate_vertex(ve[i], 'z', ROTATION_ANGLE_2);
        }
        for (int i = 0; i < VERTEX_NUMBER; ++i)
        {
            rotate_vertex(ve[i], 'x', ROTATION_ANGLE_3);
        }
        MySleep(1);
        if (SDL_PollEvent(&event) && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            break;
    }
    //SDL stop things/////////////////////////////////////////////////////////////
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}