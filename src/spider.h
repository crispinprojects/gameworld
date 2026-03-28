#ifndef SPIDER_H
#define SPIDER_H

#include "vector3.h"
#include <GL/glut.h>

class Spider {
public:
    Vector3 position;
    bool alive;
    float angle;
    float speed;
    Vector3 startPosition;
    int hitCount;
    bool chasing;

    static constexpr float SPIDER_SCALE      = 1.0f;
    static constexpr float SPIDER_RADIUS     = 0.8f * SPIDER_SCALE;
    static constexpr float SPIDER_OFFSET     = SPIDER_SCALE / 2 + 0.1f;
    static constexpr float SPIDER_BODY_WIDTH = 0.5f * SPIDER_SCALE;
    static constexpr float SPIDER_BODY_HEIGHT = 0.75f * SPIDER_SCALE;
    static constexpr float SPIDER_BODY_DEPTH  = 0.5f * SPIDER_SCALE;
    static constexpr float SPIDER_LEG_WIDTH  = 0.2f * SPIDER_SCALE;
    static constexpr float SPIDER_LEG_HEIGHT = 0.8f * SPIDER_SCALE;
    static constexpr float SPIDER_LEG_DEPTH  = 0.2f * SPIDER_SCALE;
    static constexpr float SPIDER_LEG_ANGLE  = 45.0f; // degrees

    Spider(const Vector3& pos);
    
    void update();
    void draw(float legAnimation) const;
};

#endif // SPIDER_H

