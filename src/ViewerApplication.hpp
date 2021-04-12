#pragma once

#include "utils/flag.hpp"

#include "utils/GLFWHandle.hpp"
#include "utils/cameras.hpp"
#include "utils/filesystem.hpp"

#include "utils/shaders.hpp"
#include "utils/flag.hpp"

class ViewerApplication
{
public:
  ViewerApplication(const fs::path &appPath, uint32_t width, uint32_t height,
      const std::vector<float> &lookatArgs,
      const std::string &vertexShader, const std::string &fragmentShader);
  int run();
  void window_idle();

private:
  // A range of indices in a vector containing Vertex Array Objects
  struct VaoRange
  {
    GLsizei begin; // Index of first element in vertexArrayObjects
    GLsizei count; // Number of elements in range
  };

  GLsizei m_nWindowWidth = 1280;
  GLsizei m_nWindowHeight = 720;

  const fs::path m_AppPath;
  const std::string m_AppName;
  const fs::path m_ShadersRootPath;

  std::string m_vertexShader = "forward.vs.glsl";
  std::string m_fragmentShader = "diffuse_directional_light.fs.glsl";
  
  Camera m_userCamera;
  bool m_hasUserCamera = false;

  fs::path m_OutputPath;

  // Order is important here, see comment below
  const std::string m_ImGuiIniFilename;
  // Last to be initialized, first to be destroyed:
  GLFWHandle m_GLFWHandle{int(m_nWindowWidth), int(m_nWindowHeight),
      "flag viewer",
      m_OutputPath.empty()}; // show the window only if m_OutputPath is empty
};