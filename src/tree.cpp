#include "tree.h"
#include <GL/glut.h>

Tree::Tree(const Vector3& pos) : position(pos) {}

void Tree::draw() const {
    // Trunk
    glPushMatrix();
    glTranslatef(position.x, position.y + TREE_OFFSET, position.z);
    glScalef(TRUNK_WIDTH, TRUNK_HEIGHT, TRUNK_DEPTH);
    glColor3f(0.6f, 0.4f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Leaves
    glPushMatrix();
    glTranslatef(position.x,
                 position.y + TREE_OFFSET + TRUNK_HEIGHT/2.0f + CANOPY_HEIGHT/2.0f,
                 position.z);
    glScalef(CANOPY_RADIUS, CANOPY_HEIGHT, CANOPY_RADIUS);
    glColor3f(0.0f, 0.5f, 0.0f);
    glutSolidSphere(1.0f, 10, 10);
    glPopMatrix();
}
