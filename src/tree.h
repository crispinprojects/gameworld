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
