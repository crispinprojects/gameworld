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
#include "tower.h"

Tower::Tower(const Vector3& pos) : position(pos) {}

void Tower::draw() const {        
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);  
    
    // Draw the cube (tower)
    glColor3f(1.0f, 0.0f, 0.0f); // red color 
    
    // Front face
    glBegin(GL_QUADS);
    glVertex3f(-TOWER_WIDTH/2, -TOWER_HEIGHT/2, TOWER_DEPTH/2);
    glVertex3f(TOWER_WIDTH/2, -TOWER_HEIGHT/2, TOWER_DEPTH/2);
    glVertex3f(TOWER_WIDTH/2, TOWER_HEIGHT/2, TOWER_DEPTH/2);
    glVertex3f(-TOWER_WIDTH/2, TOWER_HEIGHT/2, TOWER_DEPTH/2);
    glEnd();
    
    // Back face
    glBegin(GL_QUADS);
    glVertex3f(-TOWER_WIDTH/2, -TOWER_HEIGHT/2, -TOWER_DEPTH/2);
    glVertex3f(TOWER_WIDTH/2, -TOWER_HEIGHT/2, -TOWER_DEPTH/2);
    glVertex3f(TOWER_WIDTH/2, TOWER_HEIGHT/2, -TOWER_DEPTH/2);
    glVertex3f(-TOWER_WIDTH/2, TOWER_HEIGHT/2, -TOWER_DEPTH/2);
    glEnd();
    
    // Left face
    glBegin(GL_QUADS);
    glVertex3f(-TOWER_WIDTH/2, -TOWER_HEIGHT/2, -TOWER_DEPTH/2);
    glVertex3f(-TOWER_WIDTH/2, -TOWER_HEIGHT/2, TOWER_DEPTH/2);
    glVertex3f(-TOWER_WIDTH/2, TOWER_HEIGHT/2, TOWER_DEPTH/2);
    glVertex3f(-TOWER_WIDTH/2, TOWER_HEIGHT/2, -TOWER_DEPTH/2);
    glEnd();
    
    // Right face
    glBegin(GL_QUADS);
    glVertex3f(TOWER_WIDTH/2, -TOWER_HEIGHT/2, -TOWER_DEPTH/2);
    glVertex3f(TOWER_WIDTH/2, -TOWER_HEIGHT/2, TOWER_DEPTH/2);
    glVertex3f(TOWER_WIDTH/2, TOWER_HEIGHT/2, TOWER_DEPTH/2);
    glVertex3f(TOWER_WIDTH/2, TOWER_HEIGHT/2, -TOWER_DEPTH/2);
    glEnd();
    
    // Top face
    glBegin(GL_QUADS);
    glVertex3f(-TOWER_WIDTH/2, TOWER_HEIGHT/2, -TOWER_DEPTH/2);
    glVertex3f(TOWER_WIDTH/2, TOWER_HEIGHT/2, -TOWER_DEPTH/2);
    glVertex3f(TOWER_WIDTH/2, TOWER_HEIGHT/2, TOWER_DEPTH/2);
    glVertex3f(-TOWER_WIDTH/2, TOWER_HEIGHT/2, TOWER_DEPTH/2);
    glEnd();
    
    // Bottom face
    glBegin(GL_QUADS);
    glVertex3f(-TOWER_WIDTH/2, -TOWER_HEIGHT/2, -TOWER_DEPTH/2);
    glVertex3f(TOWER_WIDTH/2, -TOWER_HEIGHT/2, -TOWER_DEPTH/2);
    glVertex3f(TOWER_WIDTH/2, -TOWER_HEIGHT/2, TOWER_DEPTH/2);
    glVertex3f(-TOWER_WIDTH/2, -TOWER_HEIGHT/2, TOWER_DEPTH/2);
    glEnd();
    
    glPopMatrix();		
}

