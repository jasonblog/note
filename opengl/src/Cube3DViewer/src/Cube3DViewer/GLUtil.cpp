#include "GLUtil.h"

#include <GL/glew.h>

namespace GLUtil {

/* Cube Color Define
 *           ┌ ─ ─ ─ ─ ┐
 *           │ top     │
 *           │ green   │
 * ┌ ─ ─ ─ ─ ┼ ─ ─ ─ ─ ┼ ─ ─ ─ ─ ┬ ─ ─ ─ ─ ┐
 * │ left    │ front   │ right   │ back    │
 * │ blue    │ red     │ white   │ yellow  │
 * └ ─ ─ ─ ─ ┼ ─ ─ ─ ─ ┼ ─ ─ ─ ─ ┴ ─ ─ ─ ─ ┘
 *           │ bottom  │
 *           │ orange  │
 *           └ ─ ─ ─ ─ ┘
 */
void drawCube()
{
    glBegin(GL_QUADS);

    // top
    glColor3f(0.0, 1.0, 0.0); // green
    glVertex3f( 1.0, 1.0,-1.0);
    glVertex3f(-1.0, 1.0,-1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f( 1.0, 1.0, 1.0);

    // bottom
    glColor3f(1.0, 0.5, 0.0); // orange
    glVertex3f( 1.0,-1.0, 1.0);
    glVertex3f(-1.0,-1.0, 1.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glVertex3f( 1.0,-1.0,-1.0);

    // front
    glColor3f(1.0, 0.0, 0.0); // red
    glVertex3f( 1.0, 1.0, 1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(-1.0,-1.0, 1.0);
    glVertex3f( 1.0,-1.0, 1.0);

    // back
    glColor3f(1.0, 1.0, 0.0); // yellow
    glVertex3f( 1.0,-1.0,-1.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glVertex3f(-1.0, 1.0,-1.0);
    glVertex3f( 1.0, 1.0,-1.0);

    // left
    glColor3f(0.0, 0.0, 1.0); // blue
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(-1.0, 1.0,-1.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glVertex3f(-1.0,-1.0, 1.0);

    // right
    glColor3f(1.0, 1.0, 1.0); // white
    glVertex3f( 1.0, 1.0,-1.0);
    glVertex3f( 1.0, 1.0, 1.0);
    glVertex3f( 1.0,-1.0, 1.0);
    glVertex3f( 1.0,-1.0,-1.0);

    glEnd();
}

void drawAxis(float len)
{
	glBegin(GL_LINES);
	// x axis
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(len, 0.0, 0.0);

	// y axis
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, len, 0.0);

	// z axis
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, len);

	glEnd();
}

} // namespace GLUtil
