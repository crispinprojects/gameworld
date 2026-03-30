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
