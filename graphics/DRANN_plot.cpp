#define DRANN_NO_MAIN
#include "../DRANN.cpp"
#include "DRANN_plot.h"

#define GLFW_INCLUDE_NONE
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glcorearb.h>
#endif

#include "imgui.h"
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

struct DRANNPlot::Impl {
  GLFWwindow *window = nullptr;
  bool glfwReady = false, imguiReady = false, implotReady = false;
  bool platformReady = false, rendererReady = false;
  bool paused = false, logarithmic = false, autoFit = true;
  int goal = 0, epochsPerFrame = 1, pending = 0;
  drannEpoch latest{0, {0.0, 0.0}, {0.0, 0.0}, 0, 0};
  std::vector<double> epochs, training, validation;

  void initialize(int totalEpochs) {
    goal = totalEpochs;
    glfwSetErrorCallback([](int code, const char *description) {
      std::cerr << "GLFW " << code << ": " << description << '\n';
    });
    if (!glfwInit())
      throw std::runtime_error("No se pudo iniciar GLFW. Se necesita una sesión gráfica.");
    glfwReady = true;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    const char *glslVersion = "#version 150";
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    const char *glslVersion = "#version 330";
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1150, 740, "DRANN - entrenamiento en tiempo real", nullptr,
                              nullptr);
    if (!window)
      throw std::runtime_error("No se pudo crear una ventana OpenGL.");
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    imguiReady = true;
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsLight();
    ImGui::GetStyle().WindowPadding = ImVec2(20, 18);
    ImGui::GetStyle().FramePadding = ImVec2(8, 6);
    ImPlot::CreateContext();
    implotReady = true;
    ImPlot::StyleColorsLight();
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
      throw std::runtime_error("No se pudo iniciar el backend de ventana.");
    platformReady = true;
    if (!ImGui_ImplOpenGL3_Init(glslVersion))
      throw std::runtime_error("No se pudo iniciar el backend OpenGL.");
    rendererReady = true;
  }

  bool closed() const { return glfwWindowShouldClose(window) != 0; }

  void draw() {
    double frameStart = glfwGetTime();
    glfwPollEvents();
    if (closed())
      return;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::Begin("DRANN", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoSavedSettings);
    ImGui::TextUnformatted("DRANN | Evolución de la pérdida");
    ImGui::Text("Época %d / %d   |   Conexiones: %zu   |   Neuronas: %zu",
                latest.e, goal, latest.connections, latest.participants);
    ImGui::ProgressBar(static_cast<float>(latest.e) / static_cast<float>(goal),
                       ImVec2(-1, 0));
    ImGui::BeginDisabled(latest.e >= goal);
    if (ImGui::Button(paused ? "Continuar" : "Pausar"))
      paused = !paused;
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::Checkbox("Escala logarítmica", &logarithmic);
    ImGui::SameLine();
    ImGui::Checkbox("Ajustar ejes automáticamente", &autoFit);
    ImGui::SetNextItemWidth(220);
    ImGui::SliderInt("Épocas por actualización", &epochsPerFrame, 1, 100);
    ImGui::Text("Entrenamiento: %.9g    Validación: %.9g", latest.training.loss,
                latest.validation.loss);

    if (ImPlot::BeginPlot("Pérdida de entrenamiento y validación", ImVec2(-1, -70))) {
      ImPlotAxisFlags flags = autoFit ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
      ImPlot::SetupAxes("Época e", "Pérdida geométrica normalizada", flags, flags);
      if (logarithmic)
        ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
      else
        ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Linear);
      ImPlot::SetNextLineStyle(ImVec4(0.145f, 0.388f, 0.922f, 1.0f), 2.5f);
      ImPlot::PlotLine("Entrenamiento", epochs.data(), training.data(),
                       static_cast<int>(epochs.size()));
      ImPlot::SetNextLineStyle(ImVec4(0.918f, 0.345f, 0.047f, 1.0f), 2.5f);
      ImPlot::PlotLine("Validación", epochs.data(), validation.data(),
                       static_cast<int>(epochs.size()));
      ImPlot::EndPlot();
    }
    ImGui::TextUnformatted(latest.e >= goal ? "Entrenamiento completado. Puedes explorar la gráfica."
                                         : paused ? "En pausa. El modelo no se actualiza."
                                                  : "Entrenando...");
    ImGui::TextUnformatted("CSV: training_history.csv | Cerrar detiene el entrenamiento tras la época actual.");
    ImGui::End();
    ImGui::Render();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.97f, 0.98f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
    // Limitar el refresco también cuando el controlador no aplica sincronización vertical.
    double remaining = 1.0 / 60.0 - (glfwGetTime() - frameStart);
    if (remaining > 0.0)
      glfwWaitEventsTimeout(remaining);
  }

  ~Impl() {
    if (rendererReady)
      ImGui_ImplOpenGL3_Shutdown();
    if (platformReady)
      ImGui_ImplGlfw_Shutdown();
    if (implotReady)
      ImPlot::DestroyContext();
    if (imguiReady)
      ImGui::DestroyContext();
    if (window)
      glfwDestroyWindow(window);
    if (glfwReady)
      glfwTerminate();
  }
};

DRANNPlot::DRANNPlot(int epochs) : impl(std::make_unique<Impl>()) {
  impl->initialize(epochs);
}

DRANNPlot::~DRANNPlot() = default;

bool DRANNPlot::update(const drannEpoch &epoch) {
  if (impl->closed())
    return false;
  impl->latest = epoch;
  impl->epochs.push_back(epoch.e);
  impl->training.push_back(epoch.training.loss);
  impl->validation.push_back(epoch.validation.loss);
  impl->pending++;
  if (epoch.e == 0 || epoch.e == impl->goal || impl->pending >= impl->epochsPerFrame) {
    impl->pending = 0;
    impl->draw();
    while (impl->paused && !impl->closed())
      impl->draw();
  } else {
    glfwPollEvents();
  }
  return !impl->closed();
}

void DRANNPlot::finish() {
  while (!impl->closed())
    impl->draw();
}
