#ifndef WASP_H
#define WASP_H

#include "vector3.h"
#include <GL/glut.h>

class Wasp{
public:
    Vector3 position;
    bool alive;
    float angle;
    float speed;
    Vector3 startPosition;
    int hitCount;
    bool chasing = false;       
    static constexpr float WASP_SCALE   = 1.0f;
    static constexpr float WASP_HEAD_RADIUS  = 0.6f * WASP_SCALE;
    static constexpr float WASP_OFFSET  = 0.5f;    
    static constexpr float WASP_BODY_RADIUS  = 0.15f * WASP_SCALE;   
    static constexpr float WASP_BODY_HEIGHT = 1.4f * WASP_SCALE;
    static constexpr float WASP_WING_WIDTH = 0.6f * WASP_SCALE;
    static constexpr float WASP_WING_HEIGHT =0.1f * WASP_SCALE;
    static constexpr float WASP_WING_DEPTH = 2.5f * WASP_SCALE;
    static constexpr float WASP_WING_OFFSET = 0.2f * WASP_SCALE;
    
    // Helper function to draw a cylinder using quads
    void drawCylinder(float radius, float height, int slices) const;
    
    Wasp(const Vector3& pos);
    void draw(float waspAnimation) const;
    void update();
};

#endif // WASP_H
