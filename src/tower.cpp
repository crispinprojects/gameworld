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

