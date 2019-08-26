#ifndef ORBITCAMERA_H
#define ORBITCAMERA_H

#include "glm/glm.hpp"

class OrbitCamera {
public:
  OrbitCamera();
  glm::mat4 get_view_matrix() const noexcept;
  void handleMouseMove(double deltaX, double deltaY);
  void handleMouseWheel(double scrollDelta);
  void movePivotPoint(double x, double y);
  void resetPosition();

  glm::vec3 position{};

private:
  void calc_position();

  double azimuth;
  double elevation;
  double distance;
  double sensitivity;
  double scrollSpeed;
  glm::vec3 lookAtPoint{};
  glm::mat4 viewMatrix{};
};

#endif // ORBITCAMERA_H
