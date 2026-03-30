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
#include "slime.h"

Slime::Slime(const Vector3& pos)
    : position(pos), alive(true), angle(0.0f), speed(0.02f),
      startPosition(pos), hitCount(0), chasing(false) {}  

void Slime::drawCylinder(float radius, float height, int slices) const {
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++) {
        float angle = (float)i / slices * 2.0f * M_PI;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glVertex3f(x, -height/2, z);
        glVertex3f(x, height/2, z);
    }
    glEnd();
}

void Slime::draw(float slimeAnimation) const {       
    if (!alive) return;        
    glPushMatrix();
    glTranslatef(position.x, position.y + SLIME_OFFSET, position.z);   
    // Head - sphere
    glColor3f(0.0f, 0.0f, 0.0f); // black color for head
    glutSolidSphere(SLIME_HEAD_RADIUS, 10, 10);        
    // Leg - cylinder connected to head, extending vertically
    glPushMatrix();
    // Move to the bottom of the head
    glTranslatef(0.0f, SLIME_HEAD_RADIUS/2, 0.0f);
    // Rotate to make cylinder stand upright (for side-to-side wagging)
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate around X-axis for side-to-side motion
    // Position the cylinder so its bottom connects to head
    glTranslatef(0.0f, SLIME_LEG_HEIGHT/2, 0.0f);
    // Add animation - side-to-side wagging motion
    float legMotion = sin(slimeAnimation * 0.5f) * 0.3f; // Slower animation for smoother wagging
    glTranslatef(legMotion, 0.0f, 0.0f); // Move horizontally to create wagging effect
    glColor3f(0.0f, 0.0f, 0.0f); // black color for leg
    drawCylinder(SLIME_LEG_RADIUS, SLIME_LEG_HEIGHT, 10);
    glPopMatrix();        
    glPopMatrix();
}

void Slime::update() {
    if (!alive) return;
    if (chasing) return;  // do not walk while chasing

    angle += speed;
    float baseRadius = 1.0f;
    float variation  = 0.5f;
    float radius = baseRadius + variation * sin(angle * 0.3f);
    position.x = startPosition.x + radius * cos(angle);
    position.z = startPosition.z + radius * sin(angle);
}
