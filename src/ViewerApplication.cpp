#include "ViewerApplication.hpp"

#include <iostream>
#include <numeric>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

#include "utils/cameras.hpp"
#include "utils/lights.hpp"

enum MOUSE {M_NONE, MOVE};
enum WIND {W_NONE, W_X, W_Y, W_Z};
WIND wind_mod;
MOUSE mouse_action;
double lastX, lastY; // Last cursor x and y coords
const glm::mat4 identity(1.f);

Flag *flag;
glm::vec3 wind(5.0f, 5.0f, 5.0f);

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
    glfwSetWindowShouldClose(window, 1);
  }
  if(action == GLFW_PRESS){
      switch(key){
      // Modify the wind's x direction when pressing x
      case GLFW_KEY_X:
        wind_mod = W_X;
        break;

      // Modify the wind's y direction when pressing x
      case GLFW_KEY_Y:
        wind_mod = W_Y;
        break;

      // Modify the wind's z direction when pressing x
      case GLFW_KEY_Z:
        wind_mod = W_Z;
        break;

      // Add or subtract
      case GLFW_KEY_EQUAL:
        if(wind_mod == W_X) {
          wind.x += 5;
          std::cout << wind.x << std::endl;
        }
        else if(wind_mod == W_Y)
          wind.y += 5;
        else if(wind_mod == W_Z)
          wind.z += 5;
        break;

      case GLFW_KEY_MINUS:
        if(wind_mod == W_X)
          wind.x -= 5;
        else if(wind_mod == W_Y)
          wind.y -= 5;
        else if(wind_mod == W_Z)
          wind.z -= 5;
        break;

      // Kill the wind
      case GLFW_KEY_O:
        wind = glm::vec3(0.f, 0.f, 0.f);
        break;
      }
  }
}

// Mouse button callback
void window_mouse(GLFWwindow* window, int button, int action, int mods){
	// Mouse released, do nothing
    if(action == GLFW_PRESS) {
      glfwGetCursorPos(window, &lastX, &lastY);
      if(button == GLFW_MOUSE_BUTTON_2){
        mouse_action = MOVE;
      }
    }

}

// Mouse cursor moved callback
void window_cursor(GLFWwindow* window, double x, double y){
	if(mouse_action == MOVE){
		// Derive the positive x in the current perspective view
		//glm::vec3 pos_x = glm::cross(m_userCamera.up(), (m_userCamera.eye() - (-m_userCamera.up())));
		// Calculate the translation matrix
		glm::vec3 shiftAmount = ((float)(x - lastX) / 4200.0f) * glm::vec3(1.0f,1.0f,1.0f) + ((float)(lastY - y) / 180.0f) * glm::vec3(1.0f,1.0f,1.0f);
		// Now shift the cloth up this much
    std::cout << "hihi" << std::endl;
		flag->translate(shiftAmount);
		// Done translating, update x and y
		lastX = x;
		lastY = y;
	}
}

int ViewerApplication::run()
{
  // Loader shaders
  const auto glslProgram = compileProgram({m_ShadersRootPath / m_vertexShader,
      m_ShadersRootPath / m_fragmentShader});

  const auto modelViewProjMatrixLocation =
      glGetUniformLocation(glslProgram.glId(), "uModelViewProjMatrix");
  const auto modelViewMatrixLocation =
      glGetUniformLocation(glslProgram.glId(), "uModelViewMatrix");
  const auto normalMatrixLocation =
      glGetUniformLocation(glslProgram.glId(), "uNormalMatrix");
  const auto uLightIntensityLocation =
      glGetUniformLocation(glslProgram.glId(), "uLightIntensity");

  GLuint uniformP, uniformV, uniformCamPos; // Projection, view, and camera position uniform locations

  GLuint VAO, VBO, NBO;
  glm::vec3 up = glm::vec3(0, 1, 0);
  glm::vec3 eye = glm::vec3(0, 0, 35);

   // Build projection matrix
  auto maxDistance = 100.f;
  const auto projMatrix =
      glm::perspective(70.f, float(m_nWindowWidth) / m_nWindowHeight,
          0.001f * maxDistance, 1.5f * maxDistance);
          
  FirstPersonCameraController cameraController{
      m_GLFWHandle.window(), 0.5f * maxDistance};
  if (m_hasUserCamera) {
    cameraController.setCamera(m_userCamera);
  } else {
    cameraController.setCamera(
        Camera{glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)});
  }


  // Light object
  DirectionalLight light;
  flag = new Flag();

  glm::vec3 lightIntensity = light.getIntensity();
  glm::vec3 lightDirection = light.getDirection();

  // Setup OpenGL state for rendering
  glEnable(GL_DEPTH_TEST);
  glslProgram.use();

  // Lambda function to draw the scene
  const auto drawScene = [&](const Camera &camera) {
    wind_mod = W_NONE;
    mouse_action = M_NONE;

    glViewport(0, 0, m_nWindowWidth, m_nWindowHeight);
    glClearColor(0.8f,0.8f,0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto viewMatrix = camera.getViewMatrix();

    if (uLightIntensityLocation >= 0) {
      glUniform3f(uLightIntensityLocation, lightIntensity[0], lightIntensity[1],
          lightIntensity[2]);
    }

    const glm::mat4 modelMatrix(1);

    const glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
    const glm::mat4 modelViewProjMatrix = projMatrix * modelViewMatrix;
    const glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelViewMatrix));

    glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(modelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjMatrix));
    glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    if(flag) {
      flag->draw();
    }

  };

  // Loop until the user closes the window
  for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); +iterationCount) {
    const auto seconds = glfwGetTime();

    const auto camera = cameraController.getCamera();    
    drawScene(camera);
    window_idle();

    // GUI code:
    imguiNewFrame();

    {
      ImGui::Begin("GUI");
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
          1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("eye: %.3f %.3f %.3f", camera.eye().x, camera.eye().y,
            camera.eye().z);
        ImGui::Text("center: %.3f %.3f %.3f", camera.center().x,
            camera.center().y, camera.center().z);
        ImGui::Text(
            "up: %.3f %.3f %.3f", camera.up().x, camera.up().y, camera.up().z);

        ImGui::Text("front: %.3f %.3f %.3f", camera.front().x, camera.front().y,
            camera.front().z);
        ImGui::Text("left: %.3f %.3f %.3f", camera.left().x, camera.left().y,
            camera.left().z);

        if (ImGui::Button("CLI camera args to clipboard")) {
          std::stringstream ss;
          ss << "--lookat " << camera.eye().x << "," << camera.eye().y << ","
             << camera.eye().z << "," << camera.center().x << ","
             << camera.center().y << "," << camera.center().z << ","
             << camera.up().x << "," << camera.up().y << "," << camera.up().z;
          const auto str = ss.str();
          glfwSetClipboardString(m_GLFWHandle.window(), str.c_str());
        }
      }

        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
          static float theta = 0.0f;
          static float phi = 0.0f;
          static glm::vec3 lightColor = light.getColor();
          static float lightIntensityFactor = light.getIntensity().x;
          static bool lightFromCamera = true;

          ImGui::Checkbox("Light from camera", &lightFromCamera);
          if (lightFromCamera) {
            light.setDirection(-camera.front());
          }

          if (ImGui::SliderFloat("Intensity", &lightIntensityFactor, 0.f, 10.f) ||
              ImGui::ColorEdit3("color", (float *)&lightColor)) {
            lightIntensity = lightIntensityFactor * lightColor;
            light.setIntensity(lightIntensity);
          }
        }
      ImGui::End();
    }

    imguiRenderFrame();

    glfwPollEvents(); // Poll for and process events

    auto ellapsedTime = glfwGetTime() - seconds;
    auto guiHasFocus =
        ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
    if (!guiHasFocus) {
      cameraController.update(float(ellapsedTime));
    }

    m_GLFWHandle.swapBuffers(); // Swap front and back buffers
  }


  // clean up allocated GL data
  glDeleteBuffers(1, &NBO);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);

  return 0;
}


// Window idle callback
void ViewerApplication::window_idle(){
	if(flag) 
    flag->update();
}


ViewerApplication::ViewerApplication(const fs::path &appPath, uint32_t width,
    uint32_t height,
    const std::vector<float> &lookatArgs, const std::string &vertexShader,
    const std::string &fragmentShader) :
    m_nWindowWidth(width),
    m_nWindowHeight(height),
    m_AppPath{appPath},
    m_AppName{m_AppPath.stem().string()},
    m_ImGuiIniFilename{m_AppName + ".imgui.ini"},
    m_ShadersRootPath{m_AppPath.parent_path() / "shaders"}
{
  if (!lookatArgs.empty()) {
    m_hasUserCamera = true;
    m_userCamera =
        Camera{glm::vec3(lookatArgs[0], lookatArgs[1], lookatArgs[2]),
            glm::vec3(lookatArgs[3], lookatArgs[4], lookatArgs[5]),
            glm::vec3(lookatArgs[6], lookatArgs[7], lookatArgs[8])};
  }

  if (!vertexShader.empty()) {
    m_vertexShader = vertexShader;
  }

  if (!fragmentShader.empty()) {
    m_fragmentShader = fragmentShader;
  }

  ImGui::GetIO().IniFilename =
      m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows
                                  // positions in this file

  glfwSetKeyCallback(m_GLFWHandle.window(), keyCallback);

	glfwSetMouseButtonCallback(m_GLFWHandle.window(), window_mouse);

	glfwSetCursorPosCallback(m_GLFWHandle.window(), window_cursor);

  printGLVersion();
}
