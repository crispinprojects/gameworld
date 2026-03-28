#include "diamondpiece.h"

DiamondPiece::DiamondPiece(const Vector3& pos) : position(pos), collected(false), isOnTower(false) {}   

void DiamondPiece::draw() const {
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


// Getter and Setter implementations
bool DiamondPiece::getIsOnTower() const {
    return isOnTower;
}

void DiamondPiece::setIsOnTower(bool ontop) {
    isOnTower = ontop;  // on top of tower
}
