#include "spider.h"
#include <GL/glut.h>
#include <cmath>

Spider::Spider(const Vector3& pos)
    : position(pos), alive(true), angle(0.0f), speed(0.02f),
      startPosition(pos), hitCount(0), chasing(false) {}

void Spider::update() {
    if (!alive || hitCount >= 4) return;
    if (chasing) return;  // do not walk while chasing

    angle += speed;
    float baseRadius = 1.0f;
    float variation  = 0.5f;
    float radius = baseRadius + variation * sin(angle * 0.3f);
    position.x = startPosition.x + radius * cos(angle);
    position.z = startPosition.z + radius * sin(angle);
}

void Spider::draw(float legAnimation) const {
    if (!alive) return;

    glPushMatrix();
    glTranslatef(position.x, position.y + SPIDER_OFFSET, position.z);

    // Body
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 0.0f);
    glutSolidSphere(SPIDER_RADIUS, 10, 10);
    glPopMatrix();

    // Eyes
    glPushMatrix();
    glTranslatef(-SPIDER_RADIUS * 0.3f, SPIDER_RADIUS * 0.9f, 0.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glutSolidSphere(SPIDER_RADIUS * 0.2f, 8, 8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(SPIDER_RADIUS * 0.3f, SPIDER_RADIUS * 0.9f, 0.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glutSolidSphere(SPIDER_RADIUS * 0.2f, 8, 8);
    glPopMatrix();

    // Legs left
    auto drawLegLeft = [](float angleDeg, float offset) {
        glPushMatrix();
        float rad = angleDeg * M_PI / 180.0f;
        float lx  = SPIDER_RADIUS * cos(rad);
        float lz  = SPIDER_RADIUS * sin(rad);
        glTranslatef(lx, 0.0f, lz);
        glRotatef(90.0f, 0, 1, 0);
        glRotatef(-SPIDER_LEG_ANGLE, 1, 0, 0);
        glTranslatef(0.0f, offset, 0.0f);
        glScalef(SPIDER_LEG_WIDTH, SPIDER_LEG_HEIGHT, SPIDER_LEG_DEPTH);
        glColor3f(0.0f, 0.0f, 0.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    };
    
     // Legs right
    auto drawLegRight = [](float angleDeg, float offset) {
        glPushMatrix();
        float rad = angleDeg * M_PI / 180.0f;
        float lx  = SPIDER_RADIUS * cos(rad);
        float lz  = SPIDER_RADIUS * sin(rad);
        glTranslatef(lx, 0.0f, lz);
        glRotatef(90.0f, 0, 1, 0);
        glRotatef(SPIDER_LEG_ANGLE, 1, 0, 0);
        glTranslatef(0.0f, offset, 0.0f);
        glScalef(SPIDER_LEG_WIDTH, SPIDER_LEG_HEIGHT, SPIDER_LEG_DEPTH);
        glColor3f(0.0f, 0.0f, 0.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    };

    drawLegLeft(0.0f, sin(legAnimation + 0) * (SPIDER_LEG_HEIGHT / 2.0f));
    drawLegLeft(90.0f, sin(legAnimation + 1) * (SPIDER_LEG_HEIGHT / 2.0f));
    drawLegRight(180.0f, sin(legAnimation + 2) * (SPIDER_LEG_HEIGHT / 2.0f));
    drawLegRight(270.0f, sin(legAnimation + 3) * (SPIDER_LEG_HEIGHT / 2.0f));

    glPopMatrix();
}
