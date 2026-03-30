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
#include "treasuretower.h"

TreasureTower::TreasureTower(const Vector3& pos) : position(pos) {}

void TreasureTower::draw() const {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glColor3f(0.8f, 0.0f, 0.0f); // Bright red

    auto drawBox = [&](float w, float h, float d) {
        glBegin(GL_QUADS);
        // Back face
        glVertex3f(-w/2, -h/2, -d/2); glVertex3f(w/2, -h/2, -d/2);
        glVertex3f(w/2, h/2, -d/2);   glVertex3f(-w/2, h/2, -d/2);
        // Left face
        glVertex3f(-w/2, -h/2, -d/2); glVertex3f(-w/2, -h/2, d/2);
        glVertex3f(-w/2, h/2, d/2);   glVertex3f(-w/2, h/2, -d/2);
        // Right face
        glVertex3f(w/2, -h/2, -d/2);  glVertex3f(w/2, -h/2, d/2);
        glVertex3f(w/2, h/2, d/2);    glVertex3f(w/2, h/2, -d/2);
        // NO FRONT FACE (The Entrance)
        // Top face
        glVertex3f(-w/2, h/2, -d/2);  glVertex3f(w/2, h/2, -d/2);
        glVertex3f(w/2, h/2, d/2);    glVertex3f(-w/2, h/2, d/2);
        glEnd();
    };

    drawBox(TOWER_WIDTH, TOWER_HEIGHT, TOWER_DEPTH);
    glPopMatrix();
}

