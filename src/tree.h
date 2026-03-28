#ifndef TREE_H
#define TREE_H

#include "vector3.h"

class Tree {
public:
    Vector3 position;
    static constexpr float TREE_SCALE      = 1.0f;
    static constexpr float TRUNK_HEIGHT    = 2.0f * TREE_SCALE;
    static constexpr float TRUNK_WIDTH     = 0.2f * TREE_SCALE;
    static constexpr float TRUNK_DEPTH     = 0.2f * TREE_SCALE;
    static constexpr float CANOPY_HEIGHT   = 1.5f * TREE_SCALE;
    static constexpr float CANOPY_RADIUS   = 1.0f * TREE_SCALE;
    static constexpr float TREE_OFFSET     = 1.0f; // matches trunk height

    Tree(const Vector3& pos);
    void draw() const;
};

#endif // TREE_H
