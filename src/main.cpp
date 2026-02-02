/* main.cpp GameWorld (C++ OpenGL mini game demo)
 *
 * Copyright 2026 Alan Crispin <crispinalan@gmail.com> *
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
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <set>
#include <random>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Global constants
const float BULLET_RADIUS = 0.1f;
const float PLAYER_PUSH_AWAY_DISTANCE = 2.5f; // Distance to push player back when hitting tower
const float SLIME_PUSH_AWAY_DISTANCE = 12.0f; // Distance to push slime away from towers
const float SLIME_TOWER_DISTANCE = 1.0f; // Minimum distance slimes can get to tower edges
const float JUMP_DISTANCE_RANGE = 2.0f; // Range of distances from cube side at which player can jump

float slimeAnimation = 0.0f;
const float SLIME_CHASE_DISTANCE = 5.0f;
const float SPIDER_CHASE_DISTANCE = 15.0f;

//global variables
bool showRadar = true;
float radarScale = 0.05f; // Scale for radar display

float spiderLegAnimation = 0.0f;

// 3D vector helper
class Vector3 {
public:
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3& o) const { return Vector3(x + o.x, y + o.y, z + o.z); }
    Vector3 operator-(const Vector3& o) const { return Vector3(x - o.x, y - o.y, z - o.z); }
    Vector3 operator*(float s) const      { return Vector3(x * s, y * s, z * s); }
    Vector3& operator+=(const Vector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vector3& operator-=(const Vector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
};

float distance(const Vector3& a, const Vector3& b) {
    float dx = a.x - b.x;
    float dz = a.z - b.z;
    return sqrt(dx*dx + dz*dz);
}

// Standard collision detection for game entities
bool checkCollision(const Vector3& pos1, const Vector3& pos2, float radius1, float radius2) {
    float dist = distance(pos1, pos2);
    return dist < (radius1 + radius2);
}

// Special range-based collision detection for tower jumping (larger radius)
bool checkTowerJumpCollision(const Vector3& playerPos, const Vector3& towerPos, float towerRadius, float jumpRange) {
    float dx = playerPos.x - towerPos.x;
    float dz = playerPos.z - towerPos.z;
    float distance = sqrt(dx*dx + dz*dz);
    return distance < (towerRadius + jumpRange);
}

// Check if point is inside a cube
bool isPointInCube(const Vector3& point, const Vector3& cubePos, float width, float height, float depth) {
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    float halfDepth = depth / 2.0f;
    
    return (point.x >= cubePos.x - halfWidth && point.x <= cubePos.x + halfWidth &&
            point.y >= cubePos.y - halfHeight && point.y <= cubePos.y + halfHeight &&
            point.z >= cubePos.z - halfDepth && point.z <= cubePos.z + halfDepth);
}

// Check if slime head is inside a tower cube
bool isSlimeHeadInTower(const Vector3& slimePos, const Vector3& towerPos, float towerWidth, float towerHeight, float towerDepth, float slimeRadius) {
    // Create a bounding box for the slime head
    float halfWidth = towerWidth / 2.0f;
    float halfHeight = towerHeight / 2.0f;
    float halfDepth = towerDepth / 2.0f;
    
    // Check if slime head position is inside the tower cube
    return (slimePos.x >= towerPos.x - halfWidth + slimeRadius && 
            slimePos.x <= towerPos.x + halfWidth - slimeRadius &&
            slimePos.y >= towerPos.y - halfHeight + slimeRadius && 
            slimePos.y <= towerPos.y + halfHeight - slimeRadius &&
            slimePos.z >= towerPos.z - halfDepth + slimeRadius && 
            slimePos.z <= towerPos.z + halfDepth - slimeRadius);
}

// Check if two slimes are colliding
bool checkSlimeCollision(const Vector3& pos1, const Vector3& pos2, float radius1, float radius2) {
    float dist = distance(pos1, pos2);
    return dist < (radius1 + radius2);
}

//======================================================================
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

    Tree(const Vector3& pos) : position(pos) {}

    void draw() const {
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
};
//======================================================================
class GoldPiece {
public:
    Vector3 position;
    bool collected;
    static constexpr float GOLD_SCALE   = 1.0f;
    static constexpr float GOLD_RADIUS  = 0.5f * GOLD_SCALE;
    static constexpr float GOLD_OFFSET  = 0.5f;

    GoldPiece(const Vector3& pos) : position(pos), collected(false) {}

    void draw() const {
        if (!collected) {
            glPushMatrix();
            glTranslatef(position.x, position.y + GOLD_OFFSET, position.z);
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow
            glutSolidSphere(GOLD_RADIUS, 10, 10);
            glPopMatrix();
        }
    }
};

//======================================================================
class Pad {
public:
    Vector3 position;    
    static constexpr float PAD_SCALE   = 2.0f;
    static constexpr float PAD_WIDTH = 0.8f * PAD_SCALE;
    static constexpr float PAD_HEIGHT = 0.2f * PAD_SCALE;
    static constexpr float PAD_DEPTH = 0.8f * PAD_SCALE;     
    static constexpr float PAD_OFFSET = 0.0f;   
    bool isComplete; // Track if pad has correct color combination
    
    Pad(const Vector3& pos) : position(pos), isComplete(false) {}
    
    // Modified draw method that accepts color information
    void draw(const std::vector<float>& colorRGB) const {        
        glPushMatrix();
        glTranslatef(position.x, position.y + PAD_OFFSET, position.z);		
        glPushMatrix();
        glTranslatef(0.0f, PAD_HEIGHT, 0.0f);
        glScalef(PAD_WIDTH, PAD_HEIGHT, PAD_DEPTH);
        
        // Use the passed color instead of hardcoded red
        glColor3f(colorRGB[0], colorRGB[1], colorRGB[2]);
        glutSolidCube(1.0f);
        glPopMatrix();  
        glPopMatrix();
    }
    
    // Simple draw method for when no color is specified (default)
    void draw() const {        
        glPushMatrix();
        glTranslatef(position.x, position.y + PAD_OFFSET, position.z);		
        glPushMatrix();
        glTranslatef(0.0f, PAD_HEIGHT, 0.0f);
        glScalef(PAD_WIDTH, PAD_HEIGHT, PAD_DEPTH);
        
        // Default to red if no color provided
        glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidCube(1.0f);
        glPopMatrix();  
        glPopMatrix();
    }
};

//======================================================================
class Spider {
public:
    Vector3 position;
    bool alive;
    float angle;
    float speed;
    Vector3 startPosition;
    int hitCount;
    bool chasing = false;

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

    Spider(const Vector3& pos)
        : position(pos), alive(true), angle(0.0f), speed(0.02f),
          startPosition(pos), hitCount(0), chasing(false) {}

    // Walking routine – skipped while chasing
    void update() {
        if (!alive || hitCount >= 4) return;
        if (chasing) return;  // do not walk while chasing

        angle += speed;
        float baseRadius = 1.0f;
        float variation  = 0.5f;
        float radius = baseRadius + variation * sin(angle * 0.3f);
        position.x = startPosition.x + radius * cos(angle);
        position.z = startPosition.z + radius * sin(angle);
    }

    void draw(float legAnimation) const {
        if (!alive) return;

        glPushMatrix();
        glTranslatef(position.x, position.y + SPIDER_OFFSET, position.z);

        // Body
        glPushMatrix();
        glColor3f(0.0f, 0.0f, 0.0f);
        glutSolidSphere(SPIDER_RADIUS, 10, 10);
        glPopMatrix();

        // Eyes
        glPushMatrix();
        glTranslatef(-SPIDER_RADIUS * 0.3f, SPIDER_RADIUS * 0.9f, 0.0f);
        glColor3f(1.0f, 1.0f, 0.0f);
        glutSolidSphere(SPIDER_RADIUS * 0.2f, 8, 8);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(SPIDER_RADIUS * 0.3f, SPIDER_RADIUS * 0.9f, 0.0f);
        glColor3f(1.0f, 1.0f, 0.0f);
        glutSolidSphere(SPIDER_RADIUS * 0.2f, 8, 8);
        glPopMatrix();

        // Legs left
        auto drawLegLeft = [](float angleDeg, float offset) {
            glPushMatrix();
            float rad = angleDeg * M_PI / 180.0f;
            float lx  = SPIDER_RADIUS * cos(rad);
            float lz  = SPIDER_RADIUS * sin(rad);
            glTranslatef(lx, 0.0f, lz);
            glRotatef(90.0f, 0, 1, 0);
            glRotatef(-SPIDER_LEG_ANGLE, 1, 0, 0);
            glTranslatef(0.0f, offset, 0.0f);
            glScalef(SPIDER_LEG_WIDTH, SPIDER_LEG_HEIGHT, SPIDER_LEG_DEPTH);
            glColor3f(0.0f, 0.0f, 0.0f);
            glutSolidCube(1.0f);
            glPopMatrix();
        };
        
         // Legs right
        auto drawLegRight = [](float angleDeg, float offset) {
            glPushMatrix();
            float rad = angleDeg * M_PI / 180.0f;
            float lx  = SPIDER_RADIUS * cos(rad);
            float lz  = SPIDER_RADIUS * sin(rad);
            glTranslatef(lx, 0.0f, lz);
            glRotatef(90.0f, 0, 1, 0);
            glRotatef(SPIDER_LEG_ANGLE, 1, 0, 0);
            glTranslatef(0.0f, offset, 0.0f);
            glScalef(SPIDER_LEG_WIDTH, SPIDER_LEG_HEIGHT, SPIDER_LEG_DEPTH);
            glColor3f(0.0f, 0.0f, 0.0f);
            glutSolidCube(1.0f);
            glPopMatrix();
        };

        drawLegLeft(0.0f, sin(legAnimation + 0) * (SPIDER_LEG_HEIGHT / 2.0f));
        drawLegLeft(90.0f, sin(legAnimation + 1) * (SPIDER_LEG_HEIGHT / 2.0f));
        drawLegRight(180.0f, sin(legAnimation + 2) * (SPIDER_LEG_HEIGHT / 2.0f));
        drawLegRight(270.0f, sin(legAnimation + 3) * (SPIDER_LEG_HEIGHT / 2.0f));

        glPopMatrix();
    }
};
//======================================================================
class DiamondPiece {
public:
    Vector3 position;
    bool collected;
    static constexpr float DIAMOND_SCALE   = 1.0f;    
    static constexpr float DIAMOND_OFFSET  = 0.5f;
    static constexpr float DIAMOND_HEIGHT  = 1.0f * DIAMOND_SCALE; // Height of diamond
    static constexpr float DIAMOND_BASE_RADIUS = 0.5f * DIAMOND_SCALE; // Base radius
    
    DiamondPiece(const Vector3& pos) : position(pos), collected(false) {}   
    
    void draw() const {
        if (!collected) {
            glPushMatrix();
            glTranslatef(position.x, position.y + DIAMOND_OFFSET, position.z);
            
            // Draw an octahedron (diamond shape)
            glColor3f(1.0f, 0.8f, 0.0f); // Gold color for diamond
            
            float baseRadius = DIAMOND_BASE_RADIUS;
            float height = DIAMOND_HEIGHT;
            
            // Define vertices of the octahedron
            // Top vertex
            float top_y = height/2.0f;
            // Bottom vertex  
            float bottom_y = -height/2.0f;
            
            // Four base vertices (square in xz plane)
            float v1_x = baseRadius;   float v1_z = 0.0f;
            float v2_x = 0.0f;         float v2_z = baseRadius;
            float v3_x = -baseRadius;  float v3_z = 0.0f;
            float v4_x = 0.0f;         float v4_z = -baseRadius;
            
            // Draw all 8 triangular faces
            glBegin(GL_TRIANGLES);
            // Front face (top, v1, v2)
            glVertex3f(0.0f, top_y, 0.0f);
            glVertex3f(v1_x, 0.0f, v1_z);
            glVertex3f(v2_x, 0.0f, v2_z);
            glEnd();
            
            glBegin(GL_TRIANGLES);
            // Right face (top, v2, v3)
            glVertex3f(0.0f, top_y, 0.0f);
            glVertex3f(v2_x, 0.0f, v2_z);
            glVertex3f(v3_x, 0.0f, v3_z);
            glEnd();
            
            glBegin(GL_TRIANGLES);
            // Back face (top, v3, v4)
            glVertex3f(0.0f, top_y, 0.0f);
            glVertex3f(v3_x, 0.0f, v3_z);
            glVertex3f(v4_x, 0.0f, v4_z);
            glEnd();
            
            glBegin(GL_TRIANGLES);
            // Left face (top, v4, v1)
            glVertex3f(0.0f, top_y, 0.0f);
            glVertex3f(v4_x, 0.0f, v4_z);
            glVertex3f(v1_x, 0.0f, v1_z);
            glEnd();
            
            glBegin(GL_TRIANGLES);
            // Bottom front face (bottom, v1, v2)
            glVertex3f(0.0f, bottom_y, 0.0f);
            glVertex3f(v1_x, 0.0f, v1_z);
            glVertex3f(v2_x, 0.0f, v2_z);
            glEnd();
            
            glBegin(GL_TRIANGLES);
            // Bottom right face (bottom, v2, v3)
            glVertex3f(0.0f, bottom_y, 0.0f);
            glVertex3f(v2_x, 0.0f, v2_z);
            glVertex3f(v3_x, 0.0f, v3_z);
            glEnd();
            
            glBegin(GL_TRIANGLES);
            // Bottom back face (bottom, v3, v4)
            glVertex3f(0.0f, bottom_y, 0.0f);
            glVertex3f(v3_x, 0.0f, v3_z);
            glVertex3f(v4_x, 0.0f, v4_z);
            glEnd();
            
            glBegin(GL_TRIANGLES);
            // Bottom left face (bottom, v4, v1)
            glVertex3f(0.0f, bottom_y, 0.0f);
            glVertex3f(v4_x, 0.0f, v4_z);
            glVertex3f(v1_x, 0.0f, v1_z);
            glEnd();
            
            // Draw edges for better visibility
            glColor3f(0.0f, 0.0f, 0.0f); // Black edges
            glLineWidth(1.0);
            glBegin(GL_LINES);
            // Top edges
            glVertex3f(0.0f, top_y, 0.0f);
            glVertex3f(v1_x, 0.0f, v1_z);
            glVertex3f(0.0f, top_y, 0.0f);
            glVertex3f(v2_x, 0.0f, v2_z);
            glVertex3f(0.0f, top_y, 0.0f);
            glVertex3f(v3_x, 0.0f, v3_z);
            glVertex3f(0.0f, top_y, 0.0f);
            glVertex3f(v4_x, 0.0f, v4_z);
            
            // Bottom edges
            glVertex3f(0.0f, bottom_y, 0.0f);
            glVertex3f(v1_x, 0.0f, v1_z);
            glVertex3f(0.0f, bottom_y, 0.0f);
            glVertex3f(v2_x, 0.0f, v2_z);
            glVertex3f(0.0f, bottom_y, 0.0f);
            glVertex3f(v3_x, 0.0f, v3_z);
            glVertex3f(0.0f, bottom_y, 0.0f);
            glVertex3f(v4_x, 0.0f, v4_z);
            
            // Vertical edges
            glVertex3f(v1_x, 0.0f, v1_z);
            glVertex3f(0.0f, bottom_y, 0.0f);
            glVertex3f(v2_x, 0.0f, v2_z);
            glVertex3f(0.0f, bottom_y, 0.0f);
            glVertex3f(v3_x, 0.0f, v3_z);
            glVertex3f(0.0f, bottom_y, 0.0f);
            glVertex3f(v4_x, 0.0f, v4_z);
            glVertex3f(0.0f, bottom_y, 0.0f);
            
            glEnd();
            
            glPopMatrix();
        }
    }
};

//======================================================================
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
    void drawCylinder(float radius, float height, int slices) const {
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
    
    Slime(const Vector3& pos)
        : position(pos), alive(true), angle(0.0f), speed(0.02f),
          startPosition(pos), hitCount(0), chasing(false) {}  
    
    void draw(float slimeAnimation) const {       
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
    
    // Slime moving
    void update() {
        if (!alive) return;
        if (chasing) return;  // do not walk while chasing

        angle += speed;
        float baseRadius = 1.0f;
        float variation  = 0.5f;
        float radius = baseRadius + variation * sin(angle * 0.3f);
        position.x = startPosition.x + radius * cos(angle);
        position.z = startPosition.z + radius * sin(angle);
    }
        
};

//======================================================================
class Tower {
// Tower cube
public:
    Vector3 position;    
    static constexpr float TOWER_WIDTH   = 6.0f;      // Increased width for walking surface
    static constexpr float TOWER_HEIGHT  = 4.0f;      // Height increased
    static constexpr float TOWER_DEPTH   = 6.0f;      // Depth of tower
    Tower(const Vector3& pos) : position(pos) {}
    
    void draw() const {        
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
};
//======================================================================
class GameState {
public:
    // Game level configuration
    enum Level { EASY, MEDIUM, HARD };
    Level currentLevel;
    
    // Configuration variables based on level
    int numberSpiders;
    int numberSlimes;   
    int numberGoldPieces;
    int numberDiamonds;
    int numberPads;
    int numberTowers;  // Added for tower count control
    
    Vector3 playerPosition;
    float playerAngle;    // Radians
    float playerHeight;
    bool playerAlive;   
    bool flipView;  
    int goldCount = 0;
    int collectedDiamonds=0; // Count of collected diamonds
    bool onTower;         // Flag to track if player is on tower
    float towerHeight;    // Height of the tower player is currently on
    bool towerJumpReady;  // Flag to indicate if player can jump to tower
    bool canJump;         // Flag to control if player can jump (puzzle requirement)
    std::string solutionColor;
    
    struct Bullet {
        Vector3 position;
        Vector3 direction;
        float speed = 0.5f;
        bool active = true;
        Bullet(const Vector3& pos, const Vector3& dir) : position(pos), direction(dir){}
        void update() { position = position + direction * speed; }
    };
    
    // Color combination puzzle specific members
    struct PadColor {
        std::vector<std::string> colors;
        int currentColorIndex;
        PadColor() : currentColorIndex(0) {}
    };
    
    std::vector<PadColor> padColors;  // Store color combinations for each pad
    std::vector<std::string> allAvailableColors = {"Red", "Green", "Blue", "Yellow", "Purple", "Orange", "Pink"};
    
    std::vector<Tree> trees;
    std::vector<GoldPiece> goldPieces;
    std::vector<Spider> spiders;   
    std::vector<Bullet> bullets;
    std::vector<Slime> slimes;     
    std::vector<Tower> towers;      
    std::vector<DiamondPiece> diamondPieces;
    //puzzle
    std::vector<Pad> pads;
    
    // Original starting position for reset
    Vector3 originalPosition;
    float originalAngle;
       
    
    GameState()
	: playerPosition(0, 0, -14.0f), 
	   playerAngle(0.0f), 
	   playerHeight(1.0f), 
	   playerAlive(true), 
	   flipView(false), 
	   onTower(false), 
	   towerHeight(0.0f), 
	   towerJumpReady(false), 
	   canJump(false),  // Player cannot jump until puzzle is solved
	   collectedDiamonds(0),
	   currentLevel(EASY) {     
		originalPosition = playerPosition;
		originalAngle = playerAngle;
		setLevel(currentLevel);
		generateLevel();
	}

    void setLevel(Level level) {
        currentLevel = level;
        
        switch(level) {
            case EASY:
                numberSpiders = 3;
                numberSlimes = 2;   
                numberGoldPieces = 6;
                numberDiamonds = 2;
                numberPads = 3;
                numberTowers = 2;  // Fixed for all levels
                // For easy, use only first 4 colors
                allAvailableColors = {"Red", "Green", "Blue", "Yellow"};
                break;
            case MEDIUM:
                numberSpiders = 5;
                numberSlimes = 4;   
                numberGoldPieces = 10;
                numberDiamonds = 2;
                numberPads = 3;
                numberTowers = 2;  // Fixed for all levels
                // For medium, use first 5 colors
                allAvailableColors = {"Red", "Green", "Blue", "Yellow", "Purple"};
                break;
            case HARD:
                numberSpiders = 8;
                numberSlimes = 6;   
                numberGoldPieces = 15;
                numberDiamonds = 4;
                numberPads = 3;
                numberTowers = 4;  // More towers in hard level
                // For hard, use all colors
                allAvailableColors = {"Red", "Green", "Blue", "Yellow", "Purple", "Orange", "Pink"};
                break;
        }
    }
    
   
	void resetColorPuzzle() {
		padColors.clear();
		
		// Determine the solution color (random from available colors)
		//std::string solutionColor = allAvailableColors[rand() % allAvailableColors.size()];
	    solutionColor = allAvailableColors[rand() % allAvailableColors.size()];
		std::cout << "Color Puzzle Solution: " << solutionColor << std::endl;
		
		// Create pads with DIFFERENT initial colors but ALL containing the solution
		for (size_t i = 0; i < pads.size(); ++i) {
			PadColor padColor;
			
			// Create 3 unique colors - make sure they're different from each other
			std::set<std::string> selectedColors;
			
			// Add solution color first
			selectedColors.insert(solutionColor);
			
			// Add 2 more different random colors (not duplicates)
			while (selectedColors.size() < 3 && selectedColors.size() < allAvailableColors.size()) {
				std::string randomColor = allAvailableColors[rand() % allAvailableColors.size()];
				if (selectedColors.find(randomColor) == selectedColors.end()) {
					selectedColors.insert(randomColor);
				}
			}
			
			// Final safeguard to ensure 3 colors
			while (selectedColors.size() < 3) {
				std::string randomColor = allAvailableColors[rand() % allAvailableColors.size()];
				if (selectedColors.find(randomColor) == selectedColors.end()) {
					selectedColors.insert(randomColor);
				}
			}
			
			// Convert to vector for consistent storage
			padColor.colors.clear();
			for (const auto& color : selectedColors) {
				padColor.colors.push_back(color);
			}
			
			// Shuffle the colors so that the initial display isn't always the solution
			// Fixed: Using std::shuffle instead of deprecated std::random_shuffle
			std::shuffle(padColor.colors.begin(), padColor.colors.end(), std::default_random_engine(rand()));
			
			// Set current index to 0 (first displayed color)
			padColor.currentColorIndex = 0;
			padColors.push_back(padColor);
		}
		
		// This will now show different initial colors for each pad
		std::cout << "Initial display colors: ";
		for (size_t i = 0; i < padColors.size(); ++i) {
			std::cout << "Pad " << i << ": " << padColors[i].colors[padColors[i].currentColorIndex] << " ";
		}
		std::cout << std::endl;
		
		// Print all colors for debugging
		std::cout << "All color sets: ";
		for (size_t i = 0; i < padColors.size(); ++i) {
			std::cout << "Pad " << i << ": ";
			for (const auto& color : padColors[i].colors) {
				std::cout << color << " ";
			}
			std::cout << "| Current: " << padColors[i].colors[padColors[i].currentColorIndex] << " ";
		}
		std::cout << std::endl;
	}
	
	//fisher-yates shuffle
	void resetColorPuzzleFisherYates() {
    padColors.clear();
    
    // Determine the solution color (random from available colors)
    solutionColor = allAvailableColors[rand() % allAvailableColors.size()];
    std::cout << "Color Puzzle Solution: " << solutionColor << std::endl;
    
    // Create pads with DIFFERENT initial colors but ALL containing the solution
    for (size_t i = 0; i < pads.size(); ++i) {
        PadColor padColor;
        
        // Create 3 unique colors - make sure they're different from each other
        std::set<std::string> selectedColors;
        
        // Add solution color first
        selectedColors.insert(solutionColor);
        
        // Add 2 more different random colors (not duplicates)
        while (selectedColors.size() < 3 && selectedColors.size() < allAvailableColors.size()) {
            std::string randomColor = allAvailableColors[rand() % allAvailableColors.size()];
            if (selectedColors.find(randomColor) == selectedColors.end()) {
                selectedColors.insert(randomColor);
            }
        }
        
        // Final safeguard to ensure 3 colors
        while (selectedColors.size() < 3) {
            std::string randomColor = allAvailableColors[rand() % allAvailableColors.size()];
            if (selectedColors.find(randomColor) == selectedColors.end()) {
                selectedColors.insert(randomColor);
            }
        }
        
        // Convert to vector for consistent storage
        padColor.colors.clear();
        for (const auto& color : selectedColors) {
            padColor.colors.push_back(color);
        }
        
        // Shuffle the colors using a simpler method that doesn't require random header
        // Simple Fisher-Yates shuffle implementation
        for (size_t j = padColor.colors.size(); j > 1; --j) {
            size_t k = rand() % j;
            std::swap(padColor.colors[j-1], padColor.colors[k]);
        }
        
        // Set current index to 0 (first displayed color)
        padColor.currentColorIndex = 0;
        padColors.push_back(padColor);
    }
    
    // This will now show different initial colors for each pad
    std::cout << "Initial display colors: ";
    for (size_t i = 0; i < padColors.size(); ++i) {
        std::cout << "Pad " << i << ": " << padColors[i].colors[padColors[i].currentColorIndex] << " ";
    }
    std::cout << std::endl;
    
    // Print all colors for debugging
    std::cout << "All color sets: ";
    for (size_t i = 0; i < padColors.size(); ++i) {
        std::cout << "Pad " << i << ": ";
        for (const auto& color : padColors[i].colors) {
            std::cout << color << " ";
        }
        std::cout << "| Current: " << padColors[i].colors[padColors[i].currentColorIndex] << " ";
    }
    std::cout << std::endl;
}	
	
    void reset() {
        playerPosition = Vector3(0, 0, -14.0f);
        playerAngle = 0.0f;
        playerHeight = 1.0f;
        playerAlive = true; 
        onTower = false;
        towerHeight = 0.0f;
        towerJumpReady = false;  
        canJump = false;  // Reset jump ability
        collectedDiamonds = 0;
        goldCount = 0;
        resetColorPuzzle();
        generateLevel();       
    }
    
    void resetToOriginal() {
        playerPosition = originalPosition;
        playerAngle = originalAngle;
        playerHeight = 1.0f;
        playerAlive = true; 
        onTower = false;
        towerHeight = 0.0f;
        towerJumpReady = false;
        canJump = false;  // Reset jump ability
        flipView = false;       
        resetColorPuzzle();
    }
    
    void generateLevel() {       
        
        goldPieces.clear();
        spiders.clear();
        trees.clear();
        bullets.clear();
        towers.clear();
        slimes.clear();
        diamondPieces.clear();  
        pads.clear();  
        
        // Create trees in corners
        trees.emplace_back(Vector3(6, 0, 6));
        trees.emplace_back(Vector3(-6, 0, 6));
        trees.emplace_back(Vector3(6, 0, -6));
        trees.emplace_back(Vector3(-6, 0, -6));

        // Create gold pieces
        for (int i = 0; i < numberGoldPieces; ++i) {
            float x = (rand() % 30) - 15;
            float z = (rand() % 30) - 15;
            goldPieces.emplace_back(Vector3(x, 0, z));
        }

        // Create spiders
        for (int i = 0; i < numberSpiders; ++i) {
            float x = (rand() % 30) - 15;
            float z = (rand() % 30) - 15;
            spiders.emplace_back(Vector3(x, 0, z));
        }

        // Create slimes
        for (int i = 0; i < numberSlimes; ++i) {
            float x = (rand() % 30) - 15;
            float z = (rand() % 30) - 15;
            slimes.emplace_back(Vector3(x, 0, z));
        }
                   
       for (int i = 0; i < numberPads; ++i) {
			// Position them behind the first tower (at x=-20, z=4)
			float x = -25.0f - i * 3.0f;  // Position behind the first tower
			float z = 4.0f + 1.0f;        // Slightly in front of tower to make them visible
			pads.emplace_back(Vector3(x, 0, z));  // Y = 0 for ground level
		}
       
        // Create towers (using numberTowers variable)
        towers.emplace_back(Vector3(-20, 0, 4));
        towers.emplace_back(Vector3(-30, 0, 2));  
        
        // In hard level, add more towers
        if (currentLevel == HARD) {
            towers.emplace_back(Vector3(-10, 0, -10));
            towers.emplace_back(Vector3(5, 0, 15));
        }
        
        // Add diamond pieces on top of each tower
        diamondPieces.clear();
        for (const auto& tower : towers) {
            Vector3 diamondPos(tower.position.x, tower.position.y + Tower::TOWER_HEIGHT/2 + DiamondPiece::DIAMOND_OFFSET, tower.position.z);
            diamondPieces.emplace_back(diamondPos);
        }
        
         // Initialize color combinations for pads
        resetColorPuzzle();
    }  
    
    void moveForward(float d)  { playerPosition.x += sin(playerAngle) * d; playerPosition.z += cos(playerAngle) * d; }
    void moveBackward(float d) { playerPosition.x -= sin(playerAngle) * d; playerPosition.z -= cos(playerAngle) * d; }
    void turnLeft(float a)     { playerAngle += a; }
    void turnRight(float a)    { playerAngle -= a; }
    
    void updateBullets() {
        for (auto& b : bullets) if (b.active) b.update();
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b){ return !b.active; }), bullets.end());      
    }
    
    Vector3 randomPosition() const {
        // For now: uniformly in a square [-10,10] x [-10,10] on the XZ plane
        float x = (rand() / (float)RAND_MAX) * 20.0f - 10.0f;
        float z = (rand() / (float)RAND_MAX) * 20.0f - 10.0f;
        return Vector3(x, 0.0f, z);
    }

    void spawnSpider() {
        spiders.emplace_back(randomPosition());
    }    
    
    Vector3 randomPosition() {
        return Vector3((rand() % 30) - 15, 0, (rand() % 30) - 15);
    }
    
    // Check if player is over a specific pad
    int getPlayerOverPadIndex() {
        for (size_t i = 0; i < pads.size(); ++i) {
            const auto& pad = pads[i];
            float dx = playerPosition.x - pad.position.x;
            float dz = playerPosition.z - pad.position.z;
            float distance = sqrt(dx*dx + dz*dz);
            
            // Check if player is close enough to the pad (adjust threshold as needed)
            if (distance < 2.0f) {  // 2.0f is the pad detection radius
                return static_cast<int>(i);
            }
        }
        return -1;  // Not over any pad
    }
    
    // Check if puzzle is solved (all pads have same color)
    bool checkColorPuzzleSolved() {
        if (padColors.empty()) return false;
        
        std::string firstColor = padColors[0].colors[padColors[0].currentColorIndex];
        for (size_t i = 1; i < padColors.size(); ++i) {
            if (padColors[i].colors[padColors[i].currentColorIndex] != firstColor) {
                return false;
            }
        }
        return true;
    }
    
    // Cycle color on a specific pad
    void cyclePadColor(int padIndex) {
        if (padIndex >= 0 && padIndex < static_cast<int>(padColors.size())) {
            padColors[padIndex].currentColorIndex++;
            if (padColors[padIndex].currentColorIndex >= static_cast<int>(padColors[padIndex].colors.size())) {
                padColors[padIndex].currentColorIndex = 0;
            }
        }
    }
    
    // Get current color of a pad
    std::string getCurrentPadColor(int padIndex) {
        if (padIndex >= 0 && padIndex < static_cast<int>(padColors.size()) && 
            !padColors[padIndex].colors.empty()) {
            return padColors[padIndex].colors[padColors[padIndex].currentColorIndex];
        }
        return "Unknown";
    }
    
    // Get color RGB values from color name
    std::vector<float> getColorRGB(const std::string& colorName) {
        if (colorName == "Red") return {1.0f, 0.0f, 0.0f};
        if (colorName == "Green") return {0.0f, 1.0f, 0.0f};
        if (colorName == "Blue") return {0.0f, 0.0f, 1.0f};
        if (colorName == "Yellow") return {1.0f, 1.0f, 0.0f};
        if (colorName == "Purple") return {0.5f, 0.0f, 0.5f};
        if (colorName == "Orange") return {1.0f, 0.5f, 0.0f};
        if (colorName == "Pink") return {1.0f, 0.75f, 0.8f};
        return {1.0f, 0.0f, 0.0f}; // Default to red
    }
    
    // Check if all pads have same color (puzzle solved)
    bool isAllPadsSameColor() {
        if (padColors.empty()) return false;
        
        std::string firstColor = padColors[0].colors[padColors[0].currentColorIndex];
        for (size_t i = 1; i < padColors.size(); ++i) {
            if (padColors[i].colors[padColors[i].currentColorIndex] != firstColor) {
                return false;
            }
        }
        return true;
    }
};

GameState gameState;


//======================================================================
void updateSpiderAI() {
    for (auto& s : gameState.spiders) {
        if (!s.alive || s.hitCount >= 4) continue;

        Vector3 dir = gameState.playerPosition - s.position;
        float dist = std::sqrt(dir.x*dir.x + dir.z*dir.z);

        // Set chasing flag so Spider::update() skips walking
        s.chasing = (dist < SPIDER_CHASE_DISTANCE);

        if (s.chasing) {
            // Normalize direction (only X/Z)
            dir.x /= dist; dir.z /= dist;
            s.position += dir * s.speed * 0.5f;
        }

        // Random walk component – applied only when not chasing
        if (!s.chasing)
            s.angle += (rand() % 100 - 50) * 0.001f;
    }
}
//======================================================================
void drawRadar() {
    if (!showRadar) return;    
    //more work needed on radar
    // Save current state
    glPushMatrix();    
    glPushAttrib(GL_LIGHTING); // Save lighting state
    
    // Disable lighting for 2D overlay
    glDisable(GL_LIGHTING);
    
    // Set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -5, 5);  // Screen coordinates (0,0 at top-left)    
    //// Set up modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();     
    // player position (center of radar)
    
    // Draw spider positions on radar (black)
    glColor3f(0.0f, 0.0f, 0.0f);  // Black for spiders
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    for (const auto& spider : gameState.spiders) {
        if (spider.alive) {
            float dx = spider.position.x - gameState.playerPosition.x;
            float dz = spider.position.z - gameState.playerPosition.z;            
            // Convert world coordinates to radar coordinates (change sign)
            float radarX = 50 - dx * radarScale * 100;
            float radarZ = 50 - dz * radarScale * 100;            
            glVertex2f(radarX, radarZ);
        }
    }
    glEnd(); 
    
    // Draw slime positions on radar (red)
    glColor3f(1.0f, 0.0f, 0.0f);  // Red for slimes
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    for (const auto& slime : gameState.slimes) {
        if (slime.alive) {
            float dx = slime.position.x - gameState.playerPosition.x;
            float dz = slime.position.z - gameState.playerPosition.z;            
            // Convert world coordinates to radar coordinates (change sign)
            float radarX = 50 - dx * radarScale * 100;
            float radarZ = 50 - dz * radarScale * 100;            
            glVertex2f(radarX, radarZ);
        }
    }
    glEnd(); 
    
    //Draw radar grid lines
    glColor3f(0.0f, 0.0f, 0.0f);  // black for visibility
    glBegin(GL_LINES);
    for (int i = 0; i < 8; i++) {
        float angle = (float)i * M_PI / 4.0f;
        glVertex2f(50, 50);
        glVertex2f(50 + cos(angle) * 40, 50 + sin(angle) * 40);
    }
    glEnd();  
     // Draw radar circle - this should be visible
    glColor3f(1.0f, 1.0f, 1.0f);  // white
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(50, 50);  // Center of radar (top-left corner)
    for (int i = 0; i <= 32; i++) {
        float angle = (float)i * 2.0f * M_PI / 32.0f;
        glVertex2f(50 + cos(angle) * 40, 50 + sin(angle) * 40);
    }
    glEnd();  
   
   // Restore matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();    
    glPopAttrib(); // Restore lighting state
}


//======================================================================
void renderHUD() {
    // Save the current matrix state
    glPushMatrix();
    // Switch to 2D projection for HUD
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,  WINDOW_WIDTH, WINDOW_HEIGHT, 0); //screen coordinates
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
     // Render gold count and player status
    glColor3f(0.0f, 0.0f, 0.0f); // Black for HUD
    glRasterPos2f(10, WINDOW_HEIGHT - 30);    // Top-left corner
    
    char hud[100];
    sprintf(hud, "Gold Collected: %d Diamonds Collected: %d Level: %s",
            gameState.goldCount, gameState.collectedDiamonds, 
            (gameState.currentLevel == GameState::EASY) ? "EASY" : 
            (gameState.currentLevel == GameState::MEDIUM) ? "MEDIUM" : "HARD");    
    for (int i = 0; hud[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, hud[i]);
    }    
      
    // Display player status
    if (gameState.onTower) {
        glColor3f(0.0f, 0.0f, 0.0f); // Black for HUD
        glRasterPos2f(10, WINDOW_HEIGHT - 60);
        char towerMsg[100];
        sprintf(towerMsg, "Player on top of tower");    
        for (int i = 0; towerMsg[i] != '\0'; ++i) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, towerMsg[i]);
        }
    } else {
        glColor3f(0.0f, 0.0f, 0.0f); // Black for HUD
        glRasterPos2f(10, WINDOW_HEIGHT - 60);
        char groundMsg[100];
        sprintf(groundMsg, "Player at ground level");    
        for (int i = 0; groundMsg[i] != '\0'; ++i) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, groundMsg[i]);
        }
    }
    
    // Show puzzle status for color combination
    if (!gameState.canJump) {
        glColor3f(0.0f, 0.0f, 0.0f); // Black for HUD
        glRasterPos2f(10, WINDOW_HEIGHT - 90);
        char puzzleMsg[100];
        
        if(gameState.currentLevel == GameState::EASY)        
        sprintf(puzzleMsg, "Puzzle: Match colors on all pads so that they are all %s", gameState.solutionColor.c_str());
        else
        sprintf(puzzleMsg, "Puzzle: Match colors on all pads");  
        for (int i = 0; puzzleMsg[i] != '\0'; ++i) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, puzzleMsg[i]);
        }
    }
    
    // Show current color combination on each pad
    if (!gameState.canJump && gameState.padColors.size() > 0) {
        glColor3f(0.0f, 0.0f, 0.0f); // Black for HUD
        glRasterPos2f(10, WINDOW_HEIGHT - 120);
        char colorMsg[200];
        sprintf(colorMsg, "Pad Colors: ");
        int len = strlen(colorMsg);
        
        for (size_t i = 0; i < gameState.padColors.size(); ++i) {
            if (i > 0) colorMsg[len++] = ',';
            colorMsg[len++] = ' ';
            std::string currentColor = gameState.getCurrentPadColor(i);
            strcpy(&colorMsg[len], currentColor.c_str());
            len += currentColor.length();
        }
        colorMsg[len] = '\0';
        
        for (int i = 0; colorMsg[i] != '\0'; ++i) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, colorMsg[i]);
        }
    }
    
    // Show diamond collection status
    if (gameState.canJump && gameState.collectedDiamonds < static_cast<int>(gameState.diamondPieces.size())) {
        glColor3f(0.0f, 0.0f, 0.0f); // Black for HUD
        glRasterPos2f(10, WINDOW_HEIGHT - 150);
        char diamondMsg[100];
        sprintf(diamondMsg, "Diamonds to collect: %d", 
                static_cast<int>(gameState.diamondPieces.size()) - gameState.collectedDiamonds);    
        for (int i = 0; diamondMsg[i] != '\0'; ++i) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, diamondMsg[i]);
        }
    }
      
     // Show puzzle solved message
    if (gameState.canJump && gameState.isAllPadsSameColor()) {
        glColor3f(0.0f, 0.5f, 0.0f); // Green for puzzle solved message
        glRasterPos2f(10, WINDOW_HEIGHT - 210);
        char puzzleSolvedMsg[100];
        sprintf(puzzleSolvedMsg, "Color puzzle solved! Player can now jump");    
        for (int i = 0; puzzleSolvedMsg[i] != '\0'; ++i) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, puzzleSolvedMsg[i]);
        }
    }
    
    // Check if level is complete - both gold and diamonds collected
    bool levelComplete = (gameState.goldCount >= gameState.numberGoldPieces && 
                          gameState.collectedDiamonds >= gameState.numberDiamonds);
    
    if (levelComplete) {
        glColor3f(0.0f, 0.5f, 0.0f); // Green for level complete message
        glRasterPos2f(10, WINDOW_HEIGHT - 180);
        char levelCompleteMsg[100];
        sprintf(levelCompleteMsg, "LEVEL COMPLETE! All assets collected!");    
        for (int i = 0; levelCompleteMsg[i] != '\0'; ++i) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, levelCompleteMsg[i]);
        }
    }
    
    // Check for nearby slimes
    bool slimeNearby = false;
    for (const auto& slime : gameState.slimes) {
        if (!slime.alive) continue;
        float dist = distance(slime.position, gameState.playerPosition);
        if (dist < 3.0f) { // If slime is within 3 units
            slimeNearby = true;
            break;
        }
    }
    
    if (slimeNearby) {
        glColor3f(0.0f, 0.0f, 0.0f); // Black for HUD
        glRasterPos2f(10, WINDOW_HEIGHT - 240);
        char slimeMsg[100];
        sprintf(slimeMsg, "Slime close!");    
        for (int i = 0; slimeMsg[i] != '\0'; ++i) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, slimeMsg[i]);
        }
    }
    
    // Game over message
    if (!gameState.playerAlive) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red for game over
        glRasterPos2f(10, WINDOW_HEIGHT - 270);
        char gameOverMsg[100];
        sprintf(gameOverMsg, "GAME OVER PLAYER KILLED");    
        for (int i = 0; gameOverMsg[i] != '\0'; ++i) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, gameOverMsg[i]);
        }
    }
           
    // Restore original matrix state
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

//======================================================================
      
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up 3D camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (gameState.flipView) {
        // Flip the view by 180 degrees
        gluLookAt(gameState.playerPosition.x, gameState.playerHeight, gameState.playerPosition.z,
                  gameState.playerPosition.x + sin(gameState.playerAngle + M_PI), gameState.playerHeight,
                  gameState.playerPosition.z + cos(gameState.playerAngle + M_PI),
                  0.0f, 1.0f, 0.0f);
    } else {
        gluLookAt(gameState.playerPosition.x, gameState.playerHeight, gameState.playerPosition.z,
                  gameState.playerPosition.x + sin(gameState.playerAngle), gameState.playerHeight,
                  gameState.playerPosition.z + cos(gameState.playerAngle),
                  0.0f, 1.0f, 0.0f);
    }
    
    // Draw Ground 200 quad grid - THIS MUST BE FIRST
    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.8f, 0.5f);  // Green ground
    glVertex3f(-200.0f, 0.0f, -200.0f);
    glVertex3f(200.0f, 0.0f, -200.0f);
    glVertex3f(200.0f, 0.0f, 200.0f);
    glVertex3f(-200.0f, 0.0f, 200.0f);
    glEnd();   
        
    // Draw all other objects (trees, towers, pads, etc.) - they should be at y=0 or slightly above
    for (const auto& tree : gameState.trees)  tree.draw();     
    for (const auto& tower : gameState.towers)  tower.draw();
    for (const auto& gold : gameState.goldPieces) gold.draw();   
    for (const auto& diamond : gameState.diamondPieces) diamond.draw();
    
    // Draw pads with their current colors
    for (size_t i = 0; i < gameState.pads.size(); ++i) {
        std::vector<float> padColor;
        if (i < gameState.padColors.size()) {
            std::string currentColor = gameState.getCurrentPadColor(i);
            padColor = gameState.getColorRGB(currentColor);
        } else {
            padColor = {1.0f, 0.0f, 0.0f};
        }
        gameState.pads[i].draw(padColor);
    }
    
    for (const auto& spider : gameState.spiders) {
        float legAnim = spiderLegAnimation;
        if (!spider.alive) legAnim = 0.0f;
        spider.draw(legAnim);
    }
    
    // Bullets
    for (const auto& b : gameState.bullets) if (b.active) {
        glPushMatrix();
        glTranslatef(b.position.x, b.position.y, b.position.z);
        glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidSphere(BULLET_RADIUS, 8, 8);
        glPopMatrix();
    }
    
     //Draw slimes    
    for (const auto& slime : gameState.slimes) {
        float slimeAnim = slimeAnimation;
        if (!slime.alive) slimeAnim = 0.0f;
        slime.draw(slimeAnim);
    } 
    
    // Draw 2D weapon overlay
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 600, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    // Draw weapon overlay
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(350, 550); glVertex2f(450, 550);
    glVertex2f(450, 580); glVertex2f(350, 580);
    glEnd();
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(370, 580); glVertex2f(430, 580);
    glVertex2f(430, 600); glVertex2f(370, 600);
    glEnd();
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    // Draw Radar and  HUD 
    drawRadar();
    renderHUD();
    
    glutSwapBuffers();
}

//======================================================================
void keyboard(unsigned char key, int, int) {
    // Reset always works
    if (key == 'r' || key == 'R') {
        std::cout << "Reset\n";
        gameState.reset();
        return;
    }
    
    // Reset to original position
    if (key == 'o' || key == 'O') {
        std::cout << "Reset to original position\n";
        gameState.resetToOriginal();
        return;
    }
    
    if (!gameState.playerAlive) return;
    
    switch (key) {
        case 'w': case 'W': gameState.moveForward(0.5f); break;
        case 's': case 'S': gameState.moveBackward(0.5f); break;
        case 'a': case 'A': gameState.turnLeft(0.01f); break;        
        case 'd': case 'D': gameState.turnRight(0.01f); break; 
        case 'c': case 'C': std::cout << "Reset puzzle" << std::endl; break; 
        case 'e': case 'E': 
            std::cout << "e-key pressed: Activate puzzle" << std::endl;
            // Here we can add puzzle activation logic if needed
            break;      
        case 'f': case 'F': gameState.flipView = !gameState.flipView;  break;
        case 'j': case 'J': 
            // Check if player is near any tower for jumping
            if (gameState.canJump) {  // Only allow jumping if puzzle is solved
                for (const auto& tower : gameState.towers) {
                    if (checkTowerJumpCollision(gameState.playerPosition, tower.position, Tower::TOWER_WIDTH/2, JUMP_DISTANCE_RANGE)) {
                        // Player is close enough to jump to tower top
                        gameState.onTower = true;
                        gameState.towerHeight = tower.position.y + Tower::TOWER_HEIGHT/2 + 0.5f; // Add some offset for player height
                        std::cout << "Jumping to tower top at height: " << gameState.towerHeight << std::endl;
                        break;
                    }
                }
                if (!gameState.onTower) {
                    // If not close to tower, reset to ground level
                    gameState.playerHeight = 1.0f;
                    std::cout << "Not near tower, resetting to ground level" << std::endl;
                }
            } else {
                std::cout << "Puzzle not solved yet - cannot jump!" << std::endl;
            }
            break; 
            
        case 'p': case 'P': 
            std::cout << "p-key pressed: Cycle color on pad" << std::endl;
            // Check if player is over a pad and cycle color
            {
                int padIndex = gameState.getPlayerOverPadIndex();
                if (padIndex != -1) {
                    gameState.cyclePadColor(padIndex);
                    
                    // Check if puzzle is solved after cycling colors
                    if (gameState.isAllPadsSameColor()) {
                        std::cout << "Color combination puzzle solved!" << std::endl;
                        gameState.canJump = true;  // Enable jumping when puzzle is solved
                    }
                } else {
                    std::cout << "Not over any pad to cycle colors" << std::endl;
                }
            }
            break;      
		
        case 'l': case 'L': 
            // Cycle through game levels
            if (gameState.currentLevel == GameState::EASY) {
                gameState.setLevel(GameState::MEDIUM);
                std::cout << "Switched to MEDIUM level" << std::endl;
            } else if (gameState.currentLevel == GameState::MEDIUM) {
                gameState.setLevel(GameState::HARD);
                std::cout << "Switched to HARD level" << std::endl;
            } else {
                gameState.setLevel(GameState::EASY);
                std::cout << "Switched to EASY level" << std::endl;
            }
            gameState.reset();  // Reset with new level settings
            break;
		
        case ' ':
            if (gameState.playerAlive) {
                // Fire bullet in the direction the player is facing
                Vector3 bulletDir;
                if (gameState.flipView) {
                    // When flipped, fire in the opposite direction
                    bulletDir = Vector3(-sin(gameState.playerAngle), 0, -cos(gameState.playerAngle));
                } else {
                    bulletDir = Vector3(sin(gameState.playerAngle), 0, cos(gameState.playerAngle));
                }
                gameState.bullets.emplace_back(gameState.playerPosition, bulletDir);
            }
            break;  
             
        case 'z': case 'Z': showRadar=!showRadar; break;  
        case 27: exit(0); break;
    }
    glutPostRedisplay();
}

//======================================================================
void specialKeys(int key, int, int) {
    if (!gameState.playerAlive) return;
    switch (key) {
        case GLUT_KEY_UP:    gameState.moveForward(0.5f); break;
        case GLUT_KEY_DOWN:  gameState.moveBackward(0.5f); break;
        case GLUT_KEY_LEFT:  gameState.turnLeft(0.01f); break;
        case GLUT_KEY_RIGHT: gameState.turnRight(0.01f); break;
    }
    glutPostRedisplay();
}

//======================================================================
void updateSlimeAI() {
    for (auto& slime : gameState.slimes) {
       
        Vector3 dir = gameState.playerPosition - slime.position;
        float dist = std::sqrt(dir.x*dir.x + dir.z*dir.z);

        // Set chasing flag so Spider::update() skips walking
        slime.chasing = (dist < SLIME_CHASE_DISTANCE);

        if (slime.chasing) {
            // Normalize direction (only X/Z)
            dir.x /= dist; dir.z /= dist;
            slime.position += dir * slime.speed * 0.5f;
        }

        // Random walk component – applied only when not chasing
        if (!slime.chasing)
            slime.angle += (rand() % 100 - 50) * 0.001f;
    }
}

//======================================================================

void timer(int) {        
    spiderLegAnimation += 0.1f;
    slimeAnimation += 0.1f;    
    
    updateSpiderAI();          // AI chase logic
    for (auto& spider : gameState.spiders) spider.update(); // walking when not chasing
    
    updateSlimeAI();          // AI chase logic
    for (auto& slime : gameState.slimes) slime.update(); // walking when not chasing
    
    gameState.updateBullets();
    
     // Bullet–spider collisions
    for (auto it = gameState.bullets.begin(); it != gameState.bullets.end(); ) {
        if (it->active) {
            bool hit = false;
            for (auto& s : gameState.spiders) {
                if (s.alive && s.hitCount < 4 &&
                    checkCollision(it->position, s.position, BULLET_RADIUS, Spider::SPIDER_RADIUS)) {
                    s.hitCount++;
                    it->active = false;
                    hit = true;
                    //std::cout << "Spider hit! Count: " << s.hitCount << "\n";
                    if (s.hitCount >= 4) {
                        s.alive = false;
                        //std::cout << "Spider killed!\n";

                        // Spawn new spider when one dies
                        float safeDistance = 5.0f;
                        Vector3 spawnPos;
                        bool validPos = false;

                        while (!validPos) {
                            spawnPos = gameState.randomPosition();
                            float dist = distance(gameState.playerPosition, spawnPos);
                            if (dist > safeDistance) {
                                validPos = true;
                            }
                        }

                        gameState.spiders.emplace_back(spawnPos);
                        //std::cout << "Spawned new spider at (" << spawnPos.x << "," << spawnPos.z << ")\n";
                    }
                    break;
                }
            }
            if (hit) it = gameState.bullets.erase(it);
            else ++it;
        } else {
            it = gameState.bullets.erase(it);
        }
    }
    
         // Bullet–slime collisions
    for (auto it = gameState.bullets.begin(); it != gameState.bullets.end(); ) {
        if (it->active) {
            bool hit = false;
            for (auto& slime : gameState.slimes) {
                if (slime.alive && slime.hitCount < 4 &&
                    checkCollision(it->position, slime.position, BULLET_RADIUS, Slime::SLIME_HEAD_RADIUS)) {
                    slime.hitCount++;
                    it->active = false;
                    hit = true;
                    //std::cout << "Slime hit! Count: " << slime.hitCount << "\n";
                    if (slime.hitCount >= 4) {
                        slime.alive = false;
                        //std::cout << "Slime killed!\n";

                        // Spawn new slime when one dies
                        float safeDistance = 5.0f;
                        Vector3 spawnPos;
                        bool validPos = false;

                        while (!validPos) {
                            spawnPos = gameState.randomPosition();
                            float dist = distance(gameState.playerPosition, spawnPos);
                            if (dist > safeDistance) {
                                validPos = true;
                            }
                        }

                        gameState.slimes.emplace_back(spawnPos);
                        //std::cout << "Spawned new slime at (" << spawnPos.x << "," << spawnPos.z << ")\n";
                    }
                    break;
                }
            }
            if (hit) it = gameState.bullets.erase(it);
            else ++it;
        } else {
            it = gameState.bullets.erase(it);
        }
    }

    // Gold collection
    for (auto it = gameState.goldPieces.begin(); it != gameState.goldPieces.end(); ) {
        if (!it->collected && checkCollision(gameState.playerPosition, it->position, 0.1f, GoldPiece::GOLD_RADIUS)) {
            it->collected = true;
            gameState.goldCount++;
            //std::cout << "Collected gold at (" << it->position.x << "," << it->position.z << ")\n";            
            it = gameState.goldPieces.erase(it);
        } else ++it;
    }
   
    // Player–spider collision
    for (const auto& s : gameState.spiders) {
        if (s.alive && checkCollision(gameState.playerPosition, s.position, 0.1f, Spider::SPIDER_RADIUS)) {
            //std::cout << "Player collided with spider at (" << s.position.x << "," << s.position.z << ")\n";
            gameState.playerAlive = false;
            std::cout << "Player Dead\n";
            break;
        }
    }
    
    
   // Check for player slime collision
   // Slimes can only kill player if they are at ground level
   bool playerKilled = false;
   for (const auto& slime : gameState.slimes) {
       if (!slime.alive) continue;
       
       // Check if slime is close to player
       float dist = distance(slime.position, gameState.playerPosition);
       if (dist < 0.5f) { // Collision distance
           // Only kill player if they are not on tower
           if (!gameState.onTower) {
               playerKilled = true;
               std::cout << "Player killed by slime!" << std::endl;
               break;
           }
       }
   }
   
   if (playerKilled) {
       gameState.playerAlive = false;
       std::cout << "Game Over - Player died!" << std::endl;
   }
   
   // Prevent slimes from entering tower cubes
   for (auto& slime : gameState.slimes) {
       if (!slime.alive) continue;
       
       for (const auto& tower : gameState.towers) {
           // Check if slime head is inside the tower cube
           if (isSlimeHeadInTower(slime.position, tower.position, Tower::TOWER_WIDTH, Tower::TOWER_HEIGHT, Tower::TOWER_DEPTH, Slime::SLIME_HEAD_RADIUS)) {
               // Move slime away from tower to prevent it from entering
               Vector3 dir = slime.position - tower.position;
               float dist = distance(slime.position, tower.position);
               
               if (dist > 0) {
                   dir.x /= dist; 
                   dir.z /= dist;
                   // Push slime away from tower center
                   slime.position += dir * 0.1f;
               }
           }
           
           // Additional check to prevent slimes from getting too close to tower edges
           float dx = slime.position.x - tower.position.x;
           float dz = slime.position.z - tower.position.z;
           float distanceToTowerCenter = sqrt(dx*dx + dz*dz);
           
           // Keep slimes at a minimum distance from tower edges
           if (distanceToTowerCenter < SLIME_TOWER_DISTANCE) {
               // Push slime away from tower center
               if (distanceToTowerCenter > 0.1f) { // Avoid division by zero
                   dx /= distanceToTowerCenter;
                   dz /= distanceToTowerCenter;
                   slime.position.x += dx * (SLIME_TOWER_DISTANCE - distanceToTowerCenter);
                   slime.position.z += dz * (SLIME_TOWER_DISTANCE - distanceToTowerCenter);
               }
           }
       }
   }
   
   // Handle slime-slime collisions to prevent overlapping
   for (size_t i = 0; i < gameState.slimes.size(); ++i) {
       if (!gameState.slimes[i].alive) continue;
       for (size_t j = i + 1; j < gameState.slimes.size(); ++j) {
           if (!gameState.slimes[j].alive) continue;
           if (checkSlimeCollision(gameState.slimes[i].position, gameState.slimes[j].position, 
                                  Slime::SLIME_HEAD_RADIUS, Slime::SLIME_HEAD_RADIUS)) {
               // Push slimes apart to prevent overlap
               Vector3 dir = gameState.slimes[i].position - gameState.slimes[j].position;
               float dist = distance(gameState.slimes[i].position, gameState.slimes[j].position);
               
               if (dist > 0) {
                   dir.x /= dist; 
                   dir.z /= dist;
                   // Move both slimes apart
                   gameState.slimes[i].position += dir * 0.05f;
                   gameState.slimes[j].position -= dir * 0.05f;
               }
           }
       }
   }
   
   // When player is on tower, move slimes away from towers and stop chasing
   if (gameState.onTower) {
       for (auto& slime : gameState.slimes) {
           if (!slime.alive) continue;
           
           // Stop slimes from chasing when player is on tower
           slime.chasing = false;
           
           // Move slimes away from towers by a fixed distance (5 units)
           for (const auto& tower : gameState.towers) {
               Vector3 dir = slime.position - tower.position;
               float dist = distance(slime.position, tower.position);
               
               // If slime is within 5 units of tower center, push it away
               if (dist < SLIME_PUSH_AWAY_DISTANCE) {
                   if (dist > 0) {
                       dir.x /= dist; 
                       dir.z /= dist;
                       // Push slime away from tower center by exactly 5 units
                       slime.position += dir * (SLIME_PUSH_AWAY_DISTANCE- dist);
                   }
               }
           }
       }
   }
   
   // Check for tower collision with improved logic - prevent player from walking through towers
    //bool playerBlocked = false;
    for (const auto& tower : gameState.towers) {
        // Check if player is inside the tower cube
        if (isPointInCube(gameState.playerPosition, tower.position, Tower::TOWER_WIDTH, Tower::TOWER_HEIGHT, Tower::TOWER_DEPTH)) {
            std::cout << "Player collided with tower at (" << tower.position.x << "," << tower.position.z << ")\n";
            
            // Prevent player from going inside the tower - push back
            if (!gameState.onTower) {
                std::cout << "Player blocked by tower" << std::endl;
                //playerBlocked = true;
                
                // Push player back in the direction they came from, but account for flipView
                float pushX = 0.0f;
                float pushZ = 0.0f;
                
                if (gameState.flipView) {
                    // When flipped, move in opposite direction
                    pushX = -sin(gameState.playerAngle + M_PI) * PLAYER_PUSH_AWAY_DISTANCE;
                    pushZ = -cos(gameState.playerAngle + M_PI) * PLAYER_PUSH_AWAY_DISTANCE;
                } else {
                    // Normal case
                    pushX = -sin(gameState.playerAngle) * PLAYER_PUSH_AWAY_DISTANCE;
                    pushZ = -cos(gameState.playerAngle) * PLAYER_PUSH_AWAY_DISTANCE;
                }
                
                gameState.playerPosition.x += pushX;
                gameState.playerPosition.z += pushZ;
            }
            break;
        }
    }   
   
   // Check if player has moved off the tower
   bool onTowerNow = false;
   for (const auto& tower : gameState.towers) {
       if (checkTowerJumpCollision(gameState.playerPosition, tower.position, Tower::TOWER_WIDTH/2, JUMP_DISTANCE_RANGE)) {
           onTowerNow = true;
           break;
       }
   }
   
   // If player was on tower but is no longer, reset to ground level
   if (gameState.onTower && !onTowerNow) {
       gameState.playerHeight = 1.0f;
       gameState.onTower = false;
       std::cout << "Player left tower, returning to ground level" << std::endl;
   }
   
   // If player is on tower, maintain tower height
   if (gameState.onTower) {
       gameState.playerHeight = gameState.towerHeight;
   }
   
   // Check for diamond collection - only allow when canJump is true
   for (auto& piece : gameState.diamondPieces) {
       if (!piece.collected && gameState.canJump) {  // Only collect diamonds if player can jump
           float dist = distance(gameState.playerPosition, piece.position);
           if (dist < 1.0f) { // Collect if close enough
               piece.collected = true;
               gameState.collectedDiamonds++;
               std::cout << "Diamond collected! Total: " << gameState.collectedDiamonds << std::endl;
           }
       }
   }
   
   glutPostRedisplay();
   glutTimerFunc(16, timer, 0);
}



//======================================================================
void init() {
    glClearColor(0.52f, 0.8f, 0.98f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    float light_pos[]    = {10.0f, 10.0f, 10.0f, 1.0f};
    float light_amb[]    = {0.2f, 0.2f, 0.2f, 1.0f};
    float light_diff[]   = {1.0f, 1.0f, 1.0f, 1.0f};
    float light_spec[]   = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

//======================================================================
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("ZonWorld");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
