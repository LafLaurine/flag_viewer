#include "ViewerApplication.hpp"

#include <iostream>
#include <numeric>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

#include "utils/cameras.hpp"
#include "utils/lights.hpp"
#include "utils/flag.hpp"
#include "utils/sphere.hpp"

float const FPS = 30.0f;
float const dt = 1.0f/FPS;

static bool isWireframe = false;
static bool activeSpheres = false;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
    glfwSetWindowShouldClose(window, 1);
  }
}

void checkWireframe()
{
	if(isWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

inline glm::vec3 sphereCollisionForce(float distanceToCenter, 
                                      const glm::vec3& sphereCenter,
                                      float sphereRadius, 
                                      const glm::vec3 particlePosition, 
                                      const glm::vec3& forceParticle) {
    glm::vec3 direction = glm::normalize(particlePosition - sphereCenter);
    return direction * (1 / (1 + glm::pow(distanceToCenter, 2.f)));
}

void applySphereCollision(Flag& flag, const SphereHandler& sphereHandler, float multiplier, float radiusDelta, int Nu, int Nv) {
	for (int kv = 0; kv < Nv; kv++)
	{
		for (int ku = 0; ku < Nu; ku++)
		{
      for (size_t j = 0; j < sphereHandler.positions.size(); ++j) {
        float dist = glm::distance(sphereHandler.positions[j], flag.getPosition(ku,kv));
        if (dist < sphereHandler.radius[j] + radiusDelta) {
          flag.m_forces[flag.index(ku, kv)] = sphereCollisionForce(dist, sphereHandler.positions[j], sphereHandler.radius[j], flag.getPosition(ku,kv), flag.getForces(ku,kv)) * multiplier;
        }
      }
    }
  }
}

int ViewerApplication::run()
{
  // Loader shaders
  const auto glslProgram = compileProgram({m_ShadersRootPath / m_vertexShader,
      m_ShadersRootPath / m_fragmentShader});
  
  // Setup OpenGL state for rendering
  glEnable(GL_DEPTH_TEST);
  glslProgram.use();

	int Nu = 30; 
  int Nv = 30;
	Flag flag(Nu, Nv, glslProgram);
  Sphere sphere(0.4f, 32, 16);

  const auto modelViewProjMatrixLocation =
      glGetUniformLocation(glslProgram.glId(), "uModelViewProjMatrix");
  const auto modelViewMatrixLocation =
      glGetUniformLocation(glslProgram.glId(), "uModelViewMatrix");
  const auto normalMatrixLocation =
      glGetUniformLocation(glslProgram.glId(), "uNormalMatrix");
  const auto uLightIntensityLocation =
      glGetUniformLocation(glslProgram.glId(), "uLightIntensity");
  const auto uColorLocation =
      glGetUniformLocation(glslProgram.glId(), "uColor");
  const auto uWireframeLocation = glGetUniformLocation(glslProgram.glId(), "wireframe");

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

  SphereHandler sphereHandler;
  sphereHandler.positions = {glm::vec3(0,0,0), glm::vec3(0,-2,5)};
  sphereHandler.radius = {2,2};
  static float sphereCollisionMultiplier = 1.5f;
  static float radiusDelta = 0.15f;

	static float K_struct = 30.0f;
	static float K_shear = 30.0f;
	static float K_bend = 20.0f;
	static float K_wind = 30.0f;

  // Light object
  DirectionalLight light;

  glm::vec3 lightIntensity = light.getIntensity();
  glm::vec3 lightDirection = light.getDirection();

  GLuint sphereVertexCount = sphere.getVertexCount();

  GLuint sphereVAO;

  // Sohere VBO
  glGenBuffers(1, &sphereVAO);
  glBindBuffer(GL_ARRAY_BUFFER, sphereVAO);

  glBufferData(GL_ARRAY_BUFFER, sphereVertexCount * sizeof(Sphere::Vertex), sphere.getDataPointer(), GL_STATIC_DRAW);

  // Sphere VAO
  glGenVertexArrays(1, &sphereVAO);
  glBindVertexArray(sphereVAO);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Sphere::Vertex), (const GLvoid*) offsetof(Sphere::Vertex, position));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Sphere::Vertex), (const GLvoid*) offsetof(Sphere::Vertex, normal));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Lambda function to draw the scene
  const auto drawScene = [&](const Camera &camera) {
    glViewport(0, 0, m_nWindowWidth, m_nWindowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8f,0.8f,0.8f, 1.0f);

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
    glUniform1i(uWireframeLocation, (int)isWireframe);
  };

  // Loop until the user closes the window
  for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); +iterationCount) {
    const auto seconds = glfwGetTime();

    const auto camera = cameraController.getCamera();    
    light.setDirection(-camera.front());
    drawScene(camera);    

		checkWireframe();

    if(activeSpheres)
    {
      applySphereCollision(flag, sphereHandler, sphereCollisionMultiplier, radiusDelta, Nu, Nv);
    }
    
    glBindVertexArray(sphereVAO);
    
    if (activeSpheres) {
        glUniform3f(uColorLocation, 1.0f,0.0f,0.0f);
        glDrawArrays(GL_TRIANGLES, 0, sphereVertexCount);
    }

    glBindVertexArray(0);

    glUniform3f(uColorLocation, 0.15f,0.15f,0.8f);
		flag.updateForces(K_struct,K_shear,K_bend,K_wind);
		flag.updatePosition(dt);
    flag.render();

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
          
          if (ImGui::SliderFloat("Intensity", &lightIntensityFactor, 0.f, 10.f) ||
              ImGui::ColorEdit3("color", (float *)&lightColor)) {
            lightIntensity = lightIntensityFactor * lightColor;
            light.setIntensity(lightIntensity);
          }
        }

        if (ImGui::CollapsingHeader("Wind", ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Checkbox("Enable wind", &(flag.is_wind));
          ImGui::SliderFloat("Wind strength", &(flag.wind_strength), 0.0f, 0.002f);
          ImGui::SliderFloat("Wind direction x", &(flag.wind_direction.x), -1.0f, 1.0f);
          ImGui::SliderFloat("Wind direction y", &(flag.wind_direction.y), -1.0f, 1.0f);
          ImGui::SliderFloat("Wind direction z", &(flag.wind_direction.z), -1.0f, 1.0f);
        }

        if (ImGui::CollapsingHeader("Flag forces", ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::SliderFloat("Bend", &(K_bend), 10.0f, 50.f);
          ImGui::SliderFloat("Shear", &(K_shear), 10.0f, 50.0f);
          ImGui::SliderFloat("Struct", &(K_struct), 10.0f, 50.0f);
          ImGui::SliderFloat("Wind", &(K_wind), 10.0f, 50.0f);
        }

        
        static float spherePosZ = sphere.m_positions[0].z;
        static float posZ = 0.0f;

        static float spherePosX = sphere.m_positions[0].x;
        static float posX = 0.0f;

        if (ImGui::CollapsingHeader("Sphere", ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Checkbox("Active sphere", &activeSpheres);

          ImGui::SliderFloat("Collision multiplier", &(sphereCollisionMultiplier), 0.0f, 2.0f);
          ImGui::SliderFloat("Radius", &(sphere.m_radius), 0.5f, 1.0f);
          if(ImGui::SliderFloat("Position X", &(posX), 0.0f, 5.0f)) {
            sphere.m_positions[0].x = posX;
            sphere.updatePosition(0,sphere.m_positions[0]);
          }
          if(ImGui::SliderFloat("Position Z", &(posZ), 0.0f, 5.0f)) {
            sphere.m_positions[0].z = posZ;
            sphere.updatePosition(0,sphere.m_positions[0]);
          }
        }

        ImGui::Checkbox("Wireframe", &isWireframe);

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

  return 0;
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
    m_userCamera = Camera{glm::vec3(lookatArgs[0], lookatArgs[1], lookatArgs[2]),
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
