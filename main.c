#include <GL/glut.h>

#include "game.h"

void changeSize(int w, int h) {
    glViewport (0,  0,  w,  h) ;
    glMatrixMode (GL_PROJECTION) ;
    glLoadIdentity() ;
    gluOrtho2D (0,  1,  0,  1) ;
}

int main(int argc, char **argv) {

    game_init();

	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Bubble Bobble");

	// register callbacks
	// glutDisplayFunc(game_loop);
	glutReshapeFunc(changeSize);
	glutIdleFunc(game_loop);
	
	atexit(game_quit);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}
