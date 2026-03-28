#ifndef DIAMONDPiece_H
#define DIAMONDPiece_H

#include "vector3.h"
#include <GL/glut.h>

class DiamondPiece {
public:
    Vector3 position;
    bool collected;
    static constexpr float DIAMOND_SCALE   = 1.0f;    
    static constexpr float DIAMOND_OFFSET  = 0.5f;
    static constexpr float DIAMOND_HEIGHT  = 1.0f * DIAMOND_SCALE; // Height of diamond
    static constexpr float DIAMOND_BASE_RADIUS = 0.5f * DIAMOND_SCALE; // Base radius
    
    DiamondPiece(const Vector3& pos);
    void draw() const;
    
	// member variable
    bool isOnTower; //  true if on top of tower
    // Getter and Setter for isOnTower
    bool getIsOnTower() const;
    void setIsOnTower(bool open);
};

#endif // DIAMONDPiece_H
