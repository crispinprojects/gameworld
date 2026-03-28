#include "vector3.h"
#include <cmath>

float distance(const Vector3& a, const Vector3& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y; // Added Y for 3D distance
    float dz = a.z - b.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}
