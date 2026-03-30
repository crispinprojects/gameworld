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
#include "goldpiece.h"

GoldPiece::GoldPiece(const Vector3& pos) : position(pos), collected(false) {}

void GoldPiece::draw() const {
    if (!collected) {
        glPushMatrix();
        glTranslatef(position.x, position.y + GOLD_OFFSET, position.z);
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        glutSolidSphere(GOLD_RADIUS, 10, 10);
        glPopMatrix();
    }
}
