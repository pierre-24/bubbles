#include "game.h"

Game* game = NULL;

void game_fail_exit() {
    printf("something went wrong (check log), exiting ...\n");
    game_quit();
    exit(-1);
}

Image* load_texture(const char* image_path) {
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

void game_init() {
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

    // sprites for screen
    for (int i = 0; i < SCREEN_NUMBER; ++i) {
        game->screens[i] = sprite_new(game->texture_screens, 0, i * WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
        if (game->screens[i] == NULL)
            game_fail_exit();
    }

    // load definition
    game->num_items = 0;
    game->definition_items = NULL;
    game->num_monsters = 0;
    game->definition_monsters = NULL;

    f = fopen(DEFINITION_ITEMS, "r");
    if (f == NULL) {
        write_log("! unable to open item def file %s", DEFINITION_ITEMS);
        game_fail_exit();
    }
    else
        write_log("# opening item def file %s", DEFINITION_ITEMS);

    game->definition_items = item_defs_from_file(f, game->texture_items, &(game->num_items));
    fclose(f);
    if (game->definition_items == NULL || game->num_items == 0) {
        write_log("! no game items, exiting");
        game_fail_exit();
    }

    f = fopen(DEFINITION_MONSTERS, "r");
    if (f == NULL) {
        write_log("! unable to open monster def file %s", DEFINITION_MONSTERS);
        game_fail_exit();
    }
    else
        write_log("# opening monster def file %s", DEFINITION_MONSTERS);

    game->definition_monsters = monster_defs_from_file(f, game->texture_monsters, &(game->num_monsters));
    fclose(f);
    if (game->definition_monsters == NULL || game->num_monsters == 0) {
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
        write_log("# opening level file %s", FILE_LEVELS);

    game->levels = levels_new_from_file(f, game->texture_levels, game->definition_monsters, game->num_monsters,
                                        &(game->num_levels));
    fclose(f);

    if (game->levels == NULL || game->num_levels == 0) {
        write_log("! no levels, exiting");
        game_fail_exit();
    }

    game->current_level = game->levels; // ->next;

    // dragons & monsters & bubbles
    game->bub = create_bub(game->texture_dragons, 0);

    if (game->bub == NULL)
        game_fail_exit();
    
    write_log("# created bub");

    game->monster_list = monsters_new_from_level(game->current_level);
    game->bubble_list = NULL;
    game->item_list = NULL;

    // keys
    for (int i = 0; i < E_SIZE; ++i)
        game->key_pressed[i] = false;

    // openGL
    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    write_log("# READY TO PLAY !");

    game->paused = true;
    game->done = false;
    game->current_screen = SCREEN_INSTRUCTIONS; // starts with instruction screen
}

void game_loop() {
    // KEY MANAGEMENT:
    key_update_interval(game);

    if (game->key_pressed[E_QUIT])
        exit(EXIT_SUCCESS);

    // clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    if (!game->paused) {
        game_main_input_management(game);
        game_main_update_states(game);
        game_main_draw(game);
    }

    else {
        blit_sprite(game->screens[game->current_screen], 0, 0, false, false);

        if (game->key_pressed[E_ACTION_1]) {
            game->paused = false;
            game->key_pressed[E_ACTION_1] = false;

            if (game->done)
                exit(EXIT_SUCCESS);
        }
    }

    glutSwapBuffers();

}

void game_quit() {
    // screen
    if (game != NULL) {
        // textures
        image_delete(game->texture_items);
        image_delete(game->texture_monsters);
        image_delete(game->texture_levels);
        image_delete(game->texture_dragons);
        image_delete(game->texture_screens);

        for (int j = 0; j < SCREEN_NUMBER; ++j)
            sprite_delete(game->screens[j]);

        // definitions
        if (game->definition_items != NULL)  {
            for (int i = 0; i < game->num_items; ++i) {
                item_def_delete(game->definition_items[i]);
            }

            free(game->definition_items);
        }

        if (game->definition_monsters != NULL)  {
            for (int i = 0; i < game->num_monsters; ++i) {
                monster_def_delete(game->definition_monsters[i]);
            }

            free(game->definition_monsters);
        }

        // levels
        level_delete(game->levels);

        // dragons & monster & stuffs
        dragon_delete(game->bub);
        monster_delete(game->monster_list);
        bubble_delete(game->bubble_list);
        item_delete(game->item_list);

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
        GLUT_KEY_RIGHT, E_RIGHT
};

void game_special_key_down(int key, int x, int y) {
    int keyp = E_NONE;

    for (int i = 0; i < sizeof(skey_to_internal) / 2; ++i) {
        if (key == skey_to_internal[i*2])
            keyp = skey_to_internal[i*2 +1];
    }

    if (keyp != E_NONE)
        key_down(game, keyp);
}

void game_special_key_up(int key, int x, int y) {
    int keyp = E_NONE;

    for (int i = 0; i < sizeof(skey_to_internal) / 2; ++i) {
        if (key == skey_to_internal[i*2])
            keyp = skey_to_internal[i*2 +1];
    }

    if (keyp != E_NONE)
        key_up(game, keyp);
}

char key_to_internal[] = {
        'x', E_ACTION_2,
        ' ', E_ACTION_1,
        'p', E_PAUSE,
        'm', E_SHOW_SCORE,
        'c', E_SHOW_CONTROLS,
        27, E_QUIT // esc key
};

void game_key_down(unsigned char key, int x, int y) {
    int keyp = E_NONE;

    for (int i = 0; i < sizeof(key_to_internal) / 2; ++i) {
        if (key == key_to_internal[i*2])
            keyp = key_to_internal[i*2 +1];
    }

    if (keyp != E_NONE)
        key_down(game, keyp);
}

void game_key_up(unsigned char key, int x, int y) {

    int keyp = E_NONE;

    for (int i = 0; i < sizeof(key_to_internal) / 2; ++i) {
        if (key == key_to_internal[i*2])
            keyp = key_to_internal[i*2 +1];
    }

    if (keyp != E_NONE)
        key_up(game, keyp);
}