#ifndef TREASURETOWER_H
#define TREASURETOWER_H

#include "vector3.h"
#include <GL/glut.h>

class TreasureTower {
public:
    Vector3 position;
    
    static constexpr float TOWER_WIDTH   = 12.0f; 
    static constexpr float TOWER_HEIGHT  = 6.0f;
    static constexpr float TOWER_DEPTH   = 12.0f;   

    TreasureTower(const Vector3& pos);
    void draw() const;

};

#endif // TREASURETOWER_H

