#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    int r;
    int g;
    int b;
    int a;
} Color;

typedef struct {
    Color color;
    int x;
    int y;
    int h;
    int w;
    bool is_dragable;
    bool is_concrete;
    bool is_clicked;
} Shape;

typedef struct {
    int x;
    int y;
} Coord;

typedef struct {
    Coord p0;
    Coord p1;
    Coord p2;
    Coord p3;
} Bounds;

bool is_within_bounds(int start_x, int start_y, int w, int h, int pos_x, int pos_y) {
    int x_l_bound = start_x;
    int x_r_bound = start_x + w;
    int y_t_bound = start_y;
    int y_b_bound = start_y + h;

    if ((pos_x >= x_l_bound && pos_x <= x_r_bound) && (pos_y >= y_t_bound && pos_y <= y_b_bound)) {
        return true;
    }

    return false;
}

Bounds get_bounds(Shape shape) {
    Coord p0 = { shape.x, shape.y };
    Coord p1 = { shape.x + shape.w, shape.y };
    Coord p2 = { shape.x, shape.y + shape.h };
    Coord p3 = { shape.x + shape.w, shape.y + shape.h };

    Bounds bounds = { p0, p1, p2, p3 };
    return bounds;
}

void draw_shapes(
    SDL_Renderer* renderer, 
    Shape* shapes[],
    int size
) {
    for (int s = 0; s < size; s++) {
        Shape* shape = shapes[s];
        SDL_Rect rect = {shape->x, shape->y, shape->h, shape->w};

        SDL_SetRenderDrawColor(renderer, shape->color.r, shape->color.g, shape->color.b, shape->color.a);
        SDL_RenderFillRect(renderer, &rect);
    }
}

int main(int argc, char *argv[]) {
    SDL_Window* window = NULL;
    const int WIDTH = 400;
    const int HEIGHT = 900;

    // ----------------------------------------------------------------
    // BOILERPLATE BOI
    // ----------------------------------------------------------------

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Based window", 900, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // ----------------------------------------------------------------
    // BOILERPLATE END
    // ----------------------------------------------------------------


    // Loop until the user closes the window
    SDL_Event event;
    int running = 1;

    int mouse_button_down = 0;
    int mouse_init_pos_x = 0;
    int mouse_init_pos_y = 0;

    // create rect a
    Color rect_a_color = { 203, 12, 89, 255 };
    Color rect_b_color = { 10, 189, 198, 255 };
    Color rect_c_color = { 19, 62, 124, 255 };
    Color rect_d_color = { 9, 24, 51, 255 };

    Shape rect_a = { rect_a_color, 100, 100, 100, 100, true, true, false };
    Shape rect_b = { rect_b_color, 600, 300, 100, 100, true, true, false };
    Shape rect_c = { rect_c_color, 500, 400, 100, 100, true, true, false };
    Shape rect_d = { rect_d_color, 200, 200, 100, 100, true, true, false };

    Shape* shapes[] = { &rect_a, &rect_b, &rect_c, &rect_d };

    while (running) {
        int shapes_len = sizeof(shapes) / sizeof(shapes[0]);

        SDL_SetRenderDrawColor(renderer, 0, 255, 159, 255);
        SDL_RenderClear(renderer);

        draw_shapes(renderer, shapes, shapes_len);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int cur_m_x, cur_m_y;
                SDL_GetMouseState(&cur_m_x, &cur_m_y);

                mouse_button_down = 1;
                mouse_init_pos_x = cur_m_x;
                mouse_init_pos_y = cur_m_y;

                for (int s = 0; s < shapes_len; s++) {
                    Shape* shape = shapes[s];
                    int is_in_bounds = is_within_bounds(shape->x, shape->y, shape->w, shape->h, cur_m_x, cur_m_y);

                    if (is_in_bounds) {
                        shape->is_clicked = true;
                    }
                }
            }

            if (event.type == SDL_MOUSEBUTTONUP) {
                mouse_button_down = 0;
                mouse_init_pos_x = 0;
                mouse_init_pos_y = 0;

                for (int s = 0; s < shapes_len; s++) {
                    Shape* shape = shapes[s];
                    shape->is_clicked = false;
                }
            }

            if (event.type == SDL_QUIT) {
                running = 0; // Set running to 0 to exit the loop
            }
        }

        if (mouse_button_down) {
            int cur_m_x, cur_m_y;
            SDL_GetMouseState(&cur_m_x, &cur_m_y);

            // get delta of current position compared to starting
            int x_pos_delta = cur_m_x - mouse_init_pos_x;
            int y_pos_delta = cur_m_y - mouse_init_pos_y;

            // account for delta in starting position
            // otherwise the delta becomes compounding as we aren't accounting
            // for the previous delta
            mouse_init_pos_x = mouse_init_pos_x + x_pos_delta;
            mouse_init_pos_y = mouse_init_pos_y + y_pos_delta;

            // update position with delta
            for (int s = 0; s < shapes_len; s++) {
                Shape* shape_o = shapes[s];

                if (shape_o->is_clicked) {
                    // mave outer shape no matter what bruv
                    shape_o->x = shape_o->x + x_pos_delta;
                    shape_o->y = shape_o->y + y_pos_delta;
                    Bounds bounds_o = get_bounds(*shape_o);

                    for (int si = 0; si < shapes_len; si++) {
                        if (s == si) continue;

                        Shape* shape_i = shapes[si];
                        Bounds bounds_i = get_bounds(*shape_i);

                        // check if y intersecting, then if x intersecting
                        if ((bounds_o.p0.y <= bounds_i.p2.y && bounds_o.p0.y >= bounds_i.p0.y) || 
                            (bounds_o.p2.y >= bounds_i.p0.y && bounds_o.p2.y <= bounds_i.p2.y)
                        ) {

                            if ((bounds_o.p1.x >= bounds_i.p0.x && bounds_o.p1.x <= bounds_i.p1.x) || 
                                (bounds_o.p0.x <= bounds_i.p1.x && bounds_o.p0.x >= bounds_i.p0.x)
                            ) {
                                shape_i->x = shape_i->x + x_pos_delta;
                                shape_i->y = shape_i->y + y_pos_delta;
                            }
                        }
                    }

                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

