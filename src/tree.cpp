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
#include "tree.h"
#include <GL/glut.h>

Tree::Tree(const Vector3& pos) : position(pos) {}

void Tree::draw() const {
    // Trunk
    glPushMatrix();
    glTranslatef(position.x, position.y + TREE_OFFSET, position.z);
    glScalef(TRUNK_WIDTH, TRUNK_HEIGHT, TRUNK_DEPTH);
    glColor3f(0.6f, 0.4f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Leaves
    glPushMatrix();
    glTranslatef(position.x,
                 position.y + TREE_OFFSET + TRUNK_HEIGHT/2.0f + CANOPY_HEIGHT/2.0f,
                 position.z);
    glScalef(CANOPY_RADIUS, CANOPY_HEIGHT, CANOPY_RADIUS);
    glColor3f(0.0f, 0.5f, 0.0f);
    glutSolidSphere(1.0f, 10, 10);
    glPopMatrix();
}
