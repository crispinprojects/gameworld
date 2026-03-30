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
