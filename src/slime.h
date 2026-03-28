#ifndef SLIME_H
#define SLIME_H

#include "vector3.h"
#include <GL/glut.h>

class Slime{
public:
    Vector3 position;
    bool alive;
    float angle;
    float speed;
    Vector3 startPosition;
    int hitCount;
    bool chasing = false;       
    static constexpr float SLIME_SCALE   = 1.0f;
    static constexpr float SLIME_HEAD_RADIUS  = 0.8f * SLIME_SCALE;
    static constexpr float SLIME_OFFSET  = 0.5f;    
    static constexpr float SLIME_LEG_RADIUS  = 0.15f * SLIME_SCALE;   
    static constexpr float SLIME_LEG_HEIGHT = 3.0f * SLIME_SCALE; 
    
    // Helper function to draw a cylinder using quads
    void drawCylinder(float radius, float height, int slices) const;
    
    Slime(const Vector3& pos);
    void draw(float slimeAnimation) const;
    void update();
};

#endif // SLIME_H
