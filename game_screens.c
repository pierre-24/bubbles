//
// Created by pbeaujea on 4/7/18.
//

#include "game_screens.h"

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


int welcome_screen_action = 0;

void game_welcome_screen_input_management(Game *game) {
    if (game->key_pressed[E_ACTION_1]) {
        if (welcome_screen_action == 0) {
            game_main_start(game, game->current_level);
            game->paused = false;
            game->key_interval = FRAMES_BETWEEN_KEY_REPEAT_IN_GAME;
        } else if (welcome_screen_action == 1) {
            game_set_screen(game, SCREEN_INSTRUCTIONS);
        } else if (welcome_screen_action == 2) {
            game_set_screen(game, SCREEN_SCORE);
        }

        game->key_pressed[E_ACTION_1] = false;
    }

    if (key_fired(game, E_UP) || key_fired(game, E_DOWN)) {
        welcome_screen_action += game->key_pressed[E_UP] ? -1 : 1;

        if (welcome_screen_action == -1)
            welcome_screen_action = 2;

        welcome_screen_action %= 3;
    }
}

void game_welcome_screen_draw(Game *game) {
    blit_sprite(game->screens[SCREEN_WELCOME], 0, 0, false, false);
    blit_sprite(game->game_elements[GE_ARROW], 100, WS_BASE - welcome_screen_action * WS_SHIFT, false, false);
}

int wl_position = 0;
char wl_name[SCORE_NAME_SIZE + 1] = "aaaa";

void game_win_loose_screen_input_management(Game *game) {
    if (game->key_pressed[E_ACTION_1]) {
        game->scores_list = score_insert(game->scores_list, game->bub->score, wl_name);
        game_set_screen(game, SCREEN_WELCOME);
        game->key_pressed[E_ACTION_1] = false;
    }
    
    if (key_fired(game, E_LEFT) || key_fired(game, E_RIGHT)) {
        wl_position += game->key_pressed[E_LEFT] ? -1 : 1;

        if (wl_position == -1)
            wl_position = SCORE_NAME_SIZE - 1;

        wl_position %= SCORE_NAME_SIZE;
    }
    
    if (key_fired(game, E_UP) || key_fired(game, E_DOWN)) {
        wl_name[wl_position] += game->key_pressed[E_DOWN] ? -1 : 1;

        if (wl_name[wl_position] == 'z' + 1)
            wl_name[wl_position] = 'a';
        else if (wl_name[wl_position] == 'a' - 1)
            wl_name[wl_position] = 'z';
    }
}

int bitmap_string_width(Font* font, char* s) {
    int sz = 0;
    while(*s != '\0') {
        sz += font->char_width;
        s++;
    }
    
    return sz;
}

void draw_centered(Font* font, char* s, int x, int y) {
    blit_text(font, s, x - bitmap_string_width(font, s) / 2, y);
}

void game_win_loose_screen_draw(Game *game) {
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
        
    draw_centered(game->font, buffer, WL_BASE_X, WL_BASE_Y);
    
    (*animation)->framerate = WL_FRAMERATE;
    
    animation_animate(animation);
    blit_animation(
            *animation,
            WL_BASE_X - 16,
            WL_BASE_Y + 64,
            true,
            false);
    
    sprintf(buffer, "Your final score is %d", game->bub->score);
    draw_centered(game->font, buffer, WL_BASE_X, WL_BASE_Y - 50);
    strcpy(buffer, "ENTER YOU NAME:");
    draw_centered(game->font, buffer, WL_BASE_X, WL_BASE_Y - 100);
    draw_centered(game->font, wl_name, WL_BASE_X, WL_BASE_Y - 150);
    
    strcpy(buffer, "    ");
    buffer[wl_position] = '_';
    draw_centered(game->font, buffer, WL_BASE_X, WL_BASE_Y - 155);
}

int score_cpt = 0;

void game_score_screen_draw(Game *game) {
    char buffer[BUFF_SCORE];
    Score* t = game->scores_list;
    int i = 0;

    score_cpt++;

    while (t != NULL) {
        sprintf(buffer, "%s %d", t->name, t->score);
        draw_centered(game->font, buffer, WL_BASE_X, score_cpt * SCORE_SHIFT - (i * game->font->char_height));
        i++;
        t = t->next;
    }

    if (score_cpt > (i * game->font->char_height + WINDOW_HEIGHT) / SCORE_SHIFT + 1)
        score_cpt = 0;
}

void game_set_screen(Game* game, int screen) {
    keys_reset(game);
    game->key_interval = FRAMES_BETWEEN_KEY_REPEAT_IN_SCREEENS;

    if (screen >= 0 && screen < SCREEN_NUMBER) {
        write_log("# set screen %d", screen);

        game->current_screen = screen;
        game->paused = true;

        if (screen == SCREEN_SCORE)
            score_cpt = 0;
    }
}