//
// Created by pbeaujea on 4/7/18.
//

#include "game_screens.h"

void game_set_screen(Game* game, int screen) {
    game->key_interval = FRAMES_BETWEEN_KEY_REPEAT_IN_SCREEENS;
    
    if (screen >= 0 && screen < SCREEN_NUMBER) {
        write_log("# set screen %d", screen);

        game->current_screen = screen;
        game->paused = true;
    }
}

void game_simple_screen_input_management(Game *game, bool return_to_game) {

    if (game->key_pressed[E_ACTION_1]) {
        if (return_to_game) {
            game->paused = false;
            write_log("# back to game");
        }
        else
            game_set_screen(game, SCREEN_WELCOME);

        game->key_pressed[E_ACTION_1] = false;
    }
}

void game_simple_screen_draw(Game *game) {
    blit_sprite(game->screens[game->current_screen], 0, 0, false, false);
}


void game_welcome_screen_input_management(Game *game, int* action) {

    if (game->key_pressed[E_ACTION_1]) {
        if (*action == 0) {
            game_main_start(game, game->current_level);
            game->paused = false;
            game->key_interval = FRAMES_BETWEEN_KEY_REPEAT_IN_GAME;
        } else if (*action == 1) {
            game_set_screen(game, SCREEN_INSTRUCTIONS);
        }

        game->key_pressed[E_ACTION_1] = false;
    }

    if (key_fired(game, E_UP) || key_fired(game, E_DOWN)) {
        *action += game->key_pressed[E_UP] ? -1 : 1;

        if (*action == -1)
            *action = 2;

        *action %= 3;
    }
}

void game_welcome_screen_draw(Game *game, int action) {
    blit_sprite(game->screens[SCREEN_WELCOME], 0, 0, false, false);
    blit_sprite(game->game_elements[GE_ARROW], 100, WS_BASE - action * WS_SHIFT, false, false);
}

void game_win_loose_screen_input_management(Game *game, char name[5], int* position) {
    if (game->key_pressed[E_ACTION_1]) {
        game_set_screen(game, SCREEN_WELCOME);
        game->key_pressed[E_ACTION_1] = false;
    }
    
    if (key_fired(game, E_LEFT) || key_fired(game, E_RIGHT)) {
        *position += game->key_pressed[E_LEFT] ? -1 : 1;

        if (*position == -1)
            *position = SCORE_NAME_SIZE - 1;

        *position %= SCORE_NAME_SIZE;
    }
    
    if (key_fired(game, E_UP) || key_fired(game, E_DOWN)) {
        name[*position] += game->key_pressed[E_DOWN] ? -1 : 1;

        if (name[*position] == 'z' + 1)
            name[*position] = 'a';
        else if (name[*position] == 'a' - 1)
            name[*position] = 'z';
    }
}

int bitmap_string_width(void* font, char* s) {
    int sz = 0;
    while(*s != '\0') {
        sz += glutBitmapWidth(font, *s);
        s++;
    }
    
    return sz;
}

void draw_centered(void* font, char* s, int x, int y) {
    glRasterPos2f(x - bitmap_string_width(font, s) / 2, y);
    glutBitmapString(font, (unsigned char*) s);
}

void game_win_loose_screen_draw(Game *game, char name[5], int position) {
    bool win = game->bub->life >= 0;
    Animation** animation;
    char buffer[100];
    
    if (win) {
        animation= &(game->bub->animations[DA_NORMAL]);
        strcpy(buffer, "CONGRATULATION, YOU WIN! :)");
    }
    
    else {
        animation= &(game->bub->animations[DA_HIT]);
        strcpy(buffer, "DAMN, YOU LOOSE! :(");
    }
        
    draw_centered(GLUT_BITMAP_HELVETICA_18, buffer, WL_BASE_X, WL_BASE_Y);
    
    (*animation)->framerate = WL_FRAMERATE;
    
    animation_animate(animation);
    blit_animation(
            *animation,
            WL_BASE_X - 16,
            WL_BASE_Y + 64,
            true,
            false);
    
    sprintf(buffer, "Your final score is %d", game->bub->score);
    draw_centered(GLUT_BITMAP_HELVETICA_18, buffer, WL_BASE_X, WL_BASE_Y - 50);
    strcpy(buffer, "ENTER YOU NAME:");
    draw_centered(GLUT_BITMAP_HELVETICA_18, buffer, WL_BASE_X, WL_BASE_Y - 100);
    draw_centered(GLUT_BITMAP_9_BY_15, name, WL_BASE_X, WL_BASE_Y - 150);
    
    strcpy(buffer, "    ");
    buffer[position] = '_';
    draw_centered(GLUT_BITMAP_9_BY_15, buffer, WL_BASE_X, WL_BASE_Y - 155);
}
