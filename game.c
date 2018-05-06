#include "game.h"

Game* game = NULL;

void game_fail_exit() {
    /* Exit the game in emergency, but clean up.
     * */
    printf("something went wrong (check log), exiting ...\n");
    game_quit();
    exit(-1);
}

Image* load_texture(const char* image_path) {
    /* Load a given texture. Return NULL if the texture was not loaded.
     * */
    FILE* f;

    f = fopen(image_path, "r");
    if (f == NULL) {
        write_log("! cannot open image %s", image_path);
        return NULL;
    }

    else
        write_log("# load image %s", image_path);

    Image* tex = image_new_from_file(f);
    fclose(f);

    return tex;
}

float missed_time = .0f;

void game_init() {
    /* Init the game structure, and everything in it.
     * */
    srand((unsigned int) time(NULL));
	init_log();

    FILE* f = NULL;

    game = malloc(sizeof(Game));

    write_log("# starting Bubbles!");

    // load textures
    game->texture_items = NULL;
    game->texture_monsters = NULL;
    game->texture_levels = NULL;
    game->texture_dragons = NULL;
    game->texture_screens = NULL;
    game->texture_game = NULL;

    game->texture_items = load_texture(TEXTURE_ITEMS);
    if (game->texture_items == NULL)
        game_fail_exit();

    game->texture_monsters = load_texture(TEXTURE_MONSTERS);
    if (game->texture_monsters == NULL)
        game_fail_exit();

    game->texture_levels = load_texture(TEXTURE_LEVELS);
    if (game->texture_levels == NULL)
        game_fail_exit();

    game->texture_dragons = load_texture(TEXTURE_DRAGONS);
    if (game->texture_dragons == NULL)
        game_fail_exit();

    game->texture_screens = load_texture(TEXTURE_SCREENS);
    if (game->texture_screens == NULL)
        game_fail_exit();

    game->texture_font = load_texture(TEXTURE_FONT);
    if (game->texture_font == NULL)
        game_fail_exit();

    // sprites for screen
    for (int i = 0; i < SCREEN_NUMBER; ++i) {
        game->screens[i] = sprite_new(game->texture_screens, 0, i * WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
        if (game->screens[i] == NULL)
            game_fail_exit();
    }

    game->texture_game = load_texture(TEXTURE_GAME);
    if (game->texture_game == NULL)
        game_fail_exit();

    // sprites game elements
    for (int i = 0; i < GE_NUMBER; ++i) {
        game->game_elements[i] = sprite_new(game->texture_game, 0, i * GAME_ELEMENT_HEIGHT, GAME_ELEMENT_WIDTH, GAME_ELEMENT_HEIGHT);
        if (game->game_elements[i] == NULL)
            game_fail_exit();
    }

    // font
    game->font = font_new(game->texture_font, 16, 32);
    if (game->font == NULL)
        game_fail_exit();

    // load definition
    game->num_items_defined = 0;
    game->definition_items = NULL;
    game->num_monsters_defined = 0;
    game->definition_monsters = NULL;

    f = fopen(DEFINITION_ITEMS, "r");
    if (f == NULL) {
        write_log("! unable to open item def file %s", DEFINITION_ITEMS);
        game_fail_exit();
    }
    else
        write_log("# ------> opening item def file %s", DEFINITION_ITEMS);

    game->definition_items = item_defs_from_file(f, game->texture_items, &(game->num_items_defined));
    fclose(f);
    if (game->definition_items == NULL || game->num_items_defined == 0) {
        write_log("! no game items, exiting");
        game_fail_exit();
    }

    f = fopen(DEFINITION_MONSTERS, "r");
    if (f == NULL) {
        write_log("! unable to open monster def file %s", DEFINITION_MONSTERS);
        game_fail_exit();
    }
    else
        write_log("# ------> opening monster def file %s", DEFINITION_MONSTERS);

    game->definition_monsters = monster_defs_from_file(f, game->texture_monsters, &(game->num_monsters_defined));
    fclose(f);
    if (game->definition_monsters == NULL || game->num_monsters_defined == 0) {
        write_log("! no game monsters, exiting");
        game_fail_exit();
    }

    // levels
    game->levels = NULL;
    game->num_levels = 0;

    f = fopen(FILE_LEVELS, "r");
    if (f == NULL) {
        write_log("! unable to level file %s", FILE_LEVELS);
        game_fail_exit();
    }
    else
        write_log("# ------> opening level file %s", FILE_LEVELS);

    game->levels = levels_new_from_file(f, game->texture_levels, game->definition_monsters, game->num_monsters_defined,
                                        &(game->num_levels));
    fclose(f);

    if (game->levels == NULL || game->num_levels == 0) {
        write_log("! no levels, exiting");
        game_fail_exit();
    }

    game->current_level = NULL;
    game->previous_level = NULL;
    game->starting_level = game->levels;
    game->counter_next_level = counter_new(NEXT_LEVEL_TRANSITION, false, false);
    counter_stop(game->counter_next_level);
    game->counter_end_this_level = counter_new(UNTIL_NEXT_LEVEL, false, false);
    counter_stop(game->counter_end_this_level);
    game->counter_start_level = counter_new(MAX_LEVEL_TIME, false, false);
    counter_stop(game->counter_start_level);

    // set things
    game->monster_list = NULL;
    game->item_list = NULL;
    game->bubble_list = NULL;
    game->bub = NULL;

    // scores (if the file does not exists, it will be created later)
    game->scores_list = NULL;

    f = fopen(FILE_SCORES, "r");
    if (f != NULL) {
        write_log("# ------> inserting scores from file %s", FILE_SCORES);
        game->scores_list = scores_new_from_file(f);
        fclose(f);
    }

    else
        write_log("# No scores yet");

    // keys
    keys_init(game);

    // openGL
    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // initial screen
    game->main_started = false;
    game->freeze = false;
    game_set_screen(game, SCREEN_WELCOME); // starts with welcome screen

    write_log("# READY TO START !");
}

void game_loop() {
    /* Event loop for the program (GLUT callback).
     *
     * 1. Update key states ;
     * 2. Update states (do keys management) ;
     * 3. Draw.
     * */
    
    clock_t begin = clock();
    double time_per_frame = 1.0f / FPS;

    if (game->key_pressed[E_QUIT])
        exit(EXIT_SUCCESS);

    // INPUTS
    if (!game->paused) {
        int loop = 0;
        while(loop < 1 || missed_time > time_per_frame) {
            keys_update_interval(game);
            game_main_update_states(game);
            game_main_input_management(game);
    
            if (loop > 0)
                missed_time -= time_per_frame;
            loop++;
        }
    }

    else  {
        missed_time = .0;
        
        keys_update_interval(game);
        switch (game->current_screen) {
            case SCREEN_WELCOME:
                game_welcome_screen_input_management(game);
                break;
            case SCREEN_WIN:
            case SCREEN_GAME_OVER:
                game_win_loose_screen_input_management(game);
                break;
            default:
                game_simple_screen_input_management(game, game->main_started);
        }
    }

    // DRAW
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    if (!game->paused)
        game_main_draw(game);

    else  {
        switch (game->current_screen) {
            case SCREEN_WELCOME:
                game_welcome_screen_draw(game);
                break;
            case SCREEN_WIN:
            case SCREEN_GAME_OVER:
                game_win_loose_screen_draw(game);
                break;
            case SCREEN_SCORE:
                game_score_screen_draw(game);
                break;
            default:
                game_simple_screen_draw(game);
        }
    }

    glutSwapBuffers();

    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;

    if (time_spent < time_per_frame)
        custom_usleep((int) ((time_per_frame - time_spent) * 1000.f));
    else
        missed_time += time_spent - time_per_frame;
}


void game_quit() {
    /* Free everything in the game structure, then itself (GLUT callback).
     * */

    if (game != NULL) {
        // textures
        image_delete(game->texture_items);
        image_delete(game->texture_monsters);
        image_delete(game->texture_levels);
        image_delete(game->texture_dragons);
        image_delete(game->texture_screens);
        image_delete(game->texture_game);
        image_delete(game->texture_font);

        // screens
        for (int j = 0; j < SCREEN_NUMBER; ++j)
            sprite_delete(game->screens[j]);

        // sprites game elements
        for (int i = 0; i < GE_NUMBER; ++i)
            sprite_delete(game->game_elements[i]);

        // font
        font_delete(game->font);

        // definitions
        if (game->definition_items != NULL)  {
            for (int i = 0; i < game->num_items_defined; ++i) {
                item_def_delete(game->definition_items[i]);
            }

            free(game->definition_items);
        }

        if (game->definition_monsters != NULL)  {
            for (int i = 0; i < game->num_monsters_defined; ++i) {
                monster_def_delete(game->definition_monsters[i]);
            }

            free(game->definition_monsters);
        }

        // levels
        level_delete(game->levels);
        counter_delete(game->counter_end_this_level);
        counter_delete(game->counter_next_level);
        counter_delete(game->counter_start_level);

        // dragons & monster & stuffs
        dragon_delete(game->bub);
        monster_delete(game->monster_list);
        bubble_delete(game->bubble_list);
        item_delete(game->item_list);

        // scores
        if (game->scores_list != NULL) {
            FILE *f = fopen(FILE_SCORES, "w");
            if (f != NULL) {
                scores_save_in_file(f, game->scores_list);
                fclose(f);
            }

            else
                write_log("! unable to open %s to save scores :(", FILE_SCORES);

            score_delete(game->scores_list);
        }

        // keys
        keys_delete(game);

        // and finally:
        free(game);
    }

    write_log("# quitting Bubbles!");

	close_log();
}

char skey_to_internal[] = {
        GLUT_KEY_DOWN, E_DOWN,
        GLUT_KEY_UP, E_UP,
        GLUT_KEY_LEFT, E_LEFT,
        GLUT_KEY_RIGHT, E_RIGHT,
        GLUT_KEY_F1, E_SHOW_SCORE,
        GLUT_KEY_F2, E_SHOW_CONTROLS,
};

void game_special_key_down(int key, int x, int y) {
    /* Translate GLUT keys down into internal keys down (GLUT callback).
     * */
    int keyp = E_NONE;

    for (int i = 0; i < sizeof(skey_to_internal) / 2; ++i) {
        if (key == skey_to_internal[i*2])
            keyp = skey_to_internal[i*2 +1];
    }

    if (keyp != E_NONE)
        key_down(game, keyp);
}

void game_special_key_up(int key, int x, int y) {
    /* Translate GLUT keys up into internal keys up (GLUT callback).
     * */
    int keyp = E_NONE;

    for (int i = 0; i < sizeof(skey_to_internal) / 2; ++i) {
        if (key == skey_to_internal[i*2])
            keyp = skey_to_internal[i*2 +1];
    }

    if (keyp != E_NONE)
        key_up(game, keyp);
}

char key_to_internal[] = {
        ' ', E_ACTION_1,
        'x', E_ACTION_2,
        'X', E_ACTION_2,
        'p', E_FREEZE,
        'P', E_FREEZE,
        27, E_QUIT // esc key
};

void game_key_down(unsigned char key, int x, int y) {
    /* Translate GLUT keys down into internal keys down (GLUT callback).
     * */
    int keyp = E_NONE;

    for (int i = 0; i < sizeof(key_to_internal) / 2; ++i) {
        if (key == key_to_internal[i*2])
            keyp = key_to_internal[i*2 +1];
    }

    if (keyp != E_NONE)
        key_down(game, keyp);
}

void game_key_up(unsigned char key, int x, int y) {
    /* Translate GLUT keys up into internal keys up (GLUT callback).
     * */
    int keyp = E_NONE;

    for (int i = 0; i < sizeof(key_to_internal) / 2; ++i) {
        if (key == key_to_internal[i*2])
            keyp = key_to_internal[i*2 +1];
    }

    if (keyp != E_NONE)
        key_up(game, keyp);
}
