#include "treasuretower.h"

TreasureTower::TreasureTower(const Vector3& pos) : position(pos) {}

void TreasureTower::draw() const {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glColor3f(0.8f, 0.0f, 0.0f); // Bright red

    auto drawBox = [&](float w, float h, float d) {
        glBegin(GL_QUADS);
        // Back face
        glVertex3f(-w/2, -h/2, -d/2); glVertex3f(w/2, -h/2, -d/2);
        glVertex3f(w/2, h/2, -d/2);   glVertex3f(-w/2, h/2, -d/2);
        // Left face
        glVertex3f(-w/2, -h/2, -d/2); glVertex3f(-w/2, -h/2, d/2);
        glVertex3f(-w/2, h/2, d/2);   glVertex3f(-w/2, h/2, -d/2);
        // Right face
        glVertex3f(w/2, -h/2, -d/2);  glVertex3f(w/2, -h/2, d/2);
        glVertex3f(w/2, h/2, d/2);    glVertex3f(w/2, h/2, -d/2);
        // NO FRONT FACE (The Entrance)
        // Top face
        glVertex3f(-w/2, h/2, -d/2);  glVertex3f(w/2, h/2, -d/2);
        glVertex3f(w/2, h/2, d/2);    glVertex3f(-w/2, h/2, d/2);
        glEnd();
    };

    drawBox(TOWER_WIDTH, TOWER_HEIGHT, TOWER_DEPTH);
    glPopMatrix();
}

