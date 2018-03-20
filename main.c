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
	
	atexit(game_quit);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}
