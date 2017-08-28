#include <glm/gtx/norm.hpp>

extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

bool over_ground = false;

// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 100, 5 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
// float verticalAngle = 0.0f;
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 5.0f; // 3 units / second
glm::vec3 fall_speed = glm::vec3(0.0f, 0.0f, 0.0f); // 3 units / second
float mouseSpeed = 0.005f;
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
  return ViewMatrix;
}

glm::mat4 getProjectionMatrix(){
  return ProjectionMatrix;
}

void UpdateGravity() {
  if (over_ground) fall_speed = glm::vec3(0, 0, 0);
  else fall_speed += glm::vec3(0, -0.01, 0);
  if (length2(fall_speed) > 1) fall_speed = glm::vec3(0, -1.0f, 0);

  position += fall_speed;
  if (position.y < -10.0f) {
    position.y = -10.0f;
    over_ground = true;
  } else {
    over_ground = false;
  }
}

void UpdatePlayerPos(){
  // glfwGetTime is called only once, the first time this function is called
  static double lastTime = glfwGetTime();
  
  // Compute time difference between current and last frame
  double currentTime = glfwGetTime();
  float deltaTime = float(currentTime - lastTime);
  
  // Get mouse position
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  
  // Reset mouse position for next frame
  glfwSetCursorPos(window, 1024/2, 768/2);
  
  // Compute new orientation
  horizontalAngle += mouseSpeed * float(1024/2 - xpos );
  verticalAngle   += mouseSpeed * float( 768/2 - ypos );
  if (verticalAngle < -1.57f) verticalAngle = -1.57f;
  if (verticalAngle > 1.57f) verticalAngle = 1.57f;
  
  // Direction : Spherical coordinates to Cartesian coordinates conversion
  glm::vec3 direction(
  	cos(verticalAngle) * sin(horizontalAngle), 
  	sin(verticalAngle),
  	cos(verticalAngle) * cos(horizontalAngle)
  );
  
  // Right vector
  glm::vec3 right = glm::vec3(
  	sin(horizontalAngle - 3.14f/2.0f) * 2, 
  	0,
  	cos(horizontalAngle - 3.14f/2.0f) * 2
  );

  // Front vector
  glm::vec3 front = glm::vec3(
  	sin(horizontalAngle) * 2, 
  	0,
  	cos(horizontalAngle) * 2
  );
  
  // Up vector
  glm::vec3 up = glm::cross( right, direction );
  
  // Move forward
  if (glfwGetKey( window, GLFW_KEY_W) == GLFW_PRESS){
  	position += front * deltaTime * speed;
  }
  // Move backward
  if (glfwGetKey( window, GLFW_KEY_S) == GLFW_PRESS){
  	position -= front * deltaTime * speed;
  }
  // Strafe right
  if (glfwGetKey( window, GLFW_KEY_D) == GLFW_PRESS){
  	position += right * deltaTime * speed;
  }
  // Strafe left
  if (glfwGetKey( window, GLFW_KEY_A) == GLFW_PRESS){
  	position -= right * deltaTime * speed;
  }
  if (glfwGetKey( window, GLFW_KEY_SPACE) == GLFW_PRESS){
    // position += front * deltaTime * speed;
    if (over_ground) {
      fall_speed = glm::vec3(0, 0.3f, 0);
      over_ground = false;
    }
  }
  
  // For the next frame, the "last time" will be "now"
  lastTime = currentTime;
}

void computeMatricesFromInputs(){
  // Direction : Spherical coordinates to Cartesian coordinates conversion
  glm::vec3 direction(
  	cos(verticalAngle) * sin(horizontalAngle), 
  	sin(verticalAngle),
  	cos(verticalAngle) * cos(horizontalAngle)
  );
  
  // Right vector
  glm::vec3 right = glm::vec3(
  	sin(horizontalAngle - 3.14f/2.0f) * 2, 
  	0,
  	cos(horizontalAngle - 3.14f/2.0f) * 2
  );

  // Front vector
  glm::vec3 front = glm::vec3(
  	cos(verticalAngle) * sin(horizontalAngle) * 2, 
  	0,
  	cos(verticalAngle) * cos(horizontalAngle) * 2
  );
  
  // Up vector
  glm::vec3 up = glm::cross( right, direction );

  float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.
  
  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 2000.0f);
  // Camera matrix
  ViewMatrix       = glm::lookAt(
  							position,           // Camera is here
  							position+direction, // and looks here : at the same position, plus "direction"
  							up                  // Head is up (set to 0,-1,0 to look upside-down)
  					   );
  
}

