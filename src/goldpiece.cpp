#include "goldpiece.h"

GoldPiece::GoldPiece(const Vector3& pos) : position(pos), collected(false) {}

void GoldPiece::draw() const {
    if (!collected) {
        glPushMatrix();
        glTranslatef(position.x, position.y + GOLD_OFFSET, position.z);
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        glutSolidSphere(GOLD_RADIUS, 10, 10);
        glPopMatrix();
    }
}
