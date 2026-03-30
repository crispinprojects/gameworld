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

