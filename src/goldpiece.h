#ifndef GOLDPIECE_H
#define GOLDPIECE_H

#include "vector3.h"
#include <GL/glut.h>

class GoldPiece {
public:
    Vector3 position;
    bool collected;
    static constexpr float GOLD_SCALE   = 1.0f;
    static constexpr float GOLD_RADIUS  = 0.5f * GOLD_SCALE;
    static constexpr float GOLD_OFFSET  = 0.5f;

    GoldPiece(const Vector3& pos);
    void draw() const;
};

#endif // GOLDPIECE_H
