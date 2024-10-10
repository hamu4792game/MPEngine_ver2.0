#include "ColliderFunction.h"
#include <cmath>

float ColliderFunction::DistanceToAABB(const Vector3& cameraPos, const AABB& box) {
    Vector3 closestPoint;
    closestPoint.x = std::max(box.min.x, std::min(cameraPos.x, box.max.x));
    closestPoint.y = std::max(box.min.y, std::min(cameraPos.y, box.max.y));
    closestPoint.z = std::max(box.min.z, std::min(cameraPos.z, box.max.z));

    Vector3 diff = closestPoint - cameraPos;
    return std::sqrtf(diff * diff); // ユークリッド距離
}
