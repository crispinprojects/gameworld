#include "wasp.h"

Wasp::Wasp(const Vector3& pos)
    : position(pos), alive(true), angle(0.0f), speed(0.02f),
      startPosition(pos), hitCount(0), chasing(false) {}  

void Wasp::drawCylinder(float radius, float height, int slices) const {
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

void Wasp::draw(float waspAnimation) const {       
    if (!alive) return;        
    glPushMatrix();
    glTranslatef(position.x, position.y + WASP_OFFSET, position.z);   
    // Head - sphere
    glColor3f(0.0f, 0.0f, 0.0f); // black color for head
    glutSolidSphere(WASP_HEAD_RADIUS, 10, 10);        
    // Wasp Body cylinder connected to head, extending vertically
    glPushMatrix();
    // Move to the bottom of the head and rotate to point upwards
    glTranslatef(0.0f, -WASP_HEAD_RADIUS/2, 0.0f);  
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate around X-axis to make body vertical
    glColor3f(0.0f, 0.0f, 0.0f); // black color for body
    drawCylinder(WASP_BODY_RADIUS, WASP_BODY_HEIGHT, 10);
    glPopMatrix();  
    
    // Draw single wing at the top of the body (rotating)
    glPushMatrix();
    glTranslatef(0.0f, WASP_HEAD_RADIUS/2 + WASP_WING_OFFSET, 0.0f);
    glRotatef(waspAnimation * 10.0f, 0.0f, 1.0f, 0.0f); // Rotate wings
    glScalef(WASP_WING_WIDTH, WASP_WING_HEIGHT, WASP_WING_DEPTH);
    glColor3f(0.2f, 0.2f, 0.2f); // Dark gray for wings
    glutSolidCube(1.0f);
    glPopMatrix();
          
    glPopMatrix();
}

void Wasp::update() {
    if (!alive) return;
    
    // Hover around the position (like spiders)
    angle += speed;
    float baseRadius = 1.0f;
    float variation  = 0.5f;
    float radius = baseRadius + variation * sin(angle * 0.3f);
    position.x = startPosition.x + radius * cos(angle);
    position.z = startPosition.z + radius * sin(angle);
    
    // Add slight vertical movement
    position.y = startPosition.y + 0.2f * sin(angle * 0.5f);
}
