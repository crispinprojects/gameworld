#ifndef TOWER_H
#define TOWER_H

#include "vector3.h"
#include <GL/glut.h>

class Tower {
public:
    Vector3 position;    
    static constexpr float TOWER_WIDTH   = 6.0f;      // Increased width for walking surface
    static constexpr float TOWER_HEIGHT  = 4.0f;      // Height increased
    static constexpr float TOWER_DEPTH   = 6.0f;      // Depth of tower
    Tower(const Vector3& pos);
    
    void draw() const;
};

#endif // TOWER_H

