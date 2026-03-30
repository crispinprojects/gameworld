/*
 * 
 * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GNU General Public License v3.0 or later
 */
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
