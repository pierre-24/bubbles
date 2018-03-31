#include <GL/glut.h>

#include "game.h"

void changeSize(int w, int h) {
    glViewport (0,  0,  w,  h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D (0,  WINDOW_WIDTH,  0,  WINDOW_HEIGHT) ;
}

int main(int argc, char **argv) {

	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Bubble Bobble");

    game_init();

	glutReshapeFunc(changeSize);
	glutIdleFunc(game_loop);
	glutKeyboardFunc(game_key_down);
    glutKeyboardUpFunc(game_key_up);
    glutSpecialFunc(game_special_key_down);
    glutSpecialUpFunc(game_special_key_up);

    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	
	atexit(game_quit);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}
