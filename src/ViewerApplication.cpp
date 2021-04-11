#include "ViewerApplication.hpp"

#include <iostream>
#include <numeric>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

#include "utils/cameras.hpp"
#include "utils/lights.hpp"

enum WIND {W_NONE, W_X, W_Y, W_Z};
WIND wind_mod;
Flag *flag;
glm::vec3 wind(0, 0, 0);

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
    glfwSetWindowShouldClose(window, 1);
  }
  if(key ==  GLFW_KEY_X && action == GLFW_RELEASE)
  {
    wind_mod = W_X;
    wind.x += 5;
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
          
  std::unique_ptr<CameraController> cameraController = std::make_unique<TrackballCameraController>(m_GLFWHandle.window(), 0.01f);
  if (m_hasUserCamera) {
    cameraController->setCamera(m_userCamera);
  } else {
    cameraController->setCamera(
        Camera{eye, glm::vec3(0, 0, 0), up});
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

    glViewport(0, 0, m_nWindowWidth, m_nWindowHeight);
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

    const auto camera = cameraController->getCamera();
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

        const char *items[] = {"TrackBallCamera", "FPCamera"};
        static int item_current_idx =
            0; // Here we store our selection data as an index.
        const char *combo_label =
            items[item_current_idx]; // Label to preview before opening
                                     // the combo (technically it could be
                                     // anything)
        if (ImGui::BeginCombo(
                "Choose camera", combo_label, ImGuiComboFlags_PopupAlignLeft)) {
          const auto currentCamera = cameraController->getCamera();
          for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(items[n], is_selected))
              item_current_idx = n;

            if (is_selected)
              ImGui::SetItemDefaultFocus();

            if (item_current_idx == 0) {
              cameraController = std::make_unique<TrackballCameraController>(
                  m_GLFWHandle.window(), 0.5f * maxDistance);
            }
            if (item_current_idx == 1) {
              cameraController = std::make_unique<FirstPersonCameraController>(
                  m_GLFWHandle.window(), 0.5f * maxDistance);
            }
          }
          ImGui::EndCombo();
          cameraController->setCamera(currentCamera);
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
      cameraController->update(float(ellapsedTime));
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
	if(flag) flag->update();
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

  printGLVersion();
}
