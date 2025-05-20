#include "Renderer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iomanip>
#include <sstream>

Renderer::Renderer(GLFWwindow* window) : window(window) {
    // Get window dimensions
    glfwGetWindowSize(window, &width, &height);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Create grid and text renderer
    grid = new Grid(100.0f, 1.0f);
    textRenderer = new TextRenderer(width, height);
    
    // Initialize text renderer
    if (!textRenderer->init()) {
        std::cerr << "Warning: Failed to initialize text renderer" << std::endl;
    }
}

Renderer::~Renderer() {
    delete grid;
    delete textRenderer;
    glDeleteProgram(shaderProgram);
}

bool Renderer::initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    if (!createShaders()) {
        return false;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    return true;
}

bool Renderer::createShaders() {
    // Read shader files
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;
    
    vShaderFile.open("shaders/vertex.glsl");
    fShaderFile.open("shaders/fragment.glsl");
    
    if (!vShaderFile.is_open() || !fShaderFile.is_open()) {
        std::cerr << "Failed to open shader files" << std::endl;
        return false;
    }

    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    // Compile shaders
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);

    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Get uniform locations
    modelLoc = glGetUniformLocation(shaderProgram, "model");
    viewLoc = glGetUniformLocation(shaderProgram, "view");
    projLoc = glGetUniformLocation(shaderProgram, "projection");
    colorLoc = glGetUniformLocation(shaderProgram, "color");

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void Renderer::render(const World& world) {
    clear();

    // Update matrices
    updateViewMatrix(world.getMainCamera());
    updateProjectionMatrix();

    // Use shader program
    glUseProgram(shaderProgram);

    // Set view and projection matrices
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projMatrix);

    // Render each body with its mesh
    for (size_t i = 0; i < world.getBodyCount(); ++i) {
        const Body& body = world.getBody(i);
        const Mesh* mesh = world.getMesh(i == 0 ? "earth" : "moon");
        
        if (mesh) {
            renderMesh(*mesh, body.position, Vector(1, 1, 1));
        }
    }
}

void Renderer::render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) {
    clear();
    
    // Render grid
    grid->render(view, projection, cameraPos);
    
    // Disable depth testing for text rendering
    glDisable(GL_DEPTH_TEST);

    // Format camera position for display
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "X: " << cameraPos.x << " Y: " << cameraPos.y << " Z: " << cameraPos.z;
    
    // Calculate text position (top right corner with padding)
    float paddingRight = 20.0f; // Fixed padding from the right edge
    float paddingTop = 20.0f; // Padding from the top edge
    float textScale = 1.5f;  // Increased scale for better visibility
    // The estimatedTextWidth is not needed with this positioning approach
    // float estimatedTextWidth = ss.str().length() * 8.0f * textScale; // Estimate width based on character count

    // Render coordinates in top right corner with padding
    textRenderer->renderText(
        ss.str(),
        width - paddingRight - textRenderer->getTextWidth(ss.str(), textScale), // Position based on calculated width and right padding
        height - paddingTop,
        textScale,
        glm::vec3(1.0f, 1.0f, 1.0f)
    );

    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);
}

void Renderer::renderMesh(const Mesh& mesh, const Vector& position, const Vector& scale) {
    // Create model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
    model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));

    // Set uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(mesh.getColor().x, mesh.getColor().y, mesh.getColor().z)));

    // Render mesh
    // (Mesh rendering code will be implemented in setupMeshBuffers)
}

void Renderer::updateViewMatrix(const Camera& camera) {
    glm::vec3 pos(camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
    glm::vec3 target(camera.getTarget().x, camera.getTarget().y, camera.getTarget().z);
    glm::vec3 up(camera.getUp().x, camera.getUp().y, camera.getUp().z);
    
    glm::mat4 view = glm::lookAt(pos, target, up);
    memcpy(viewMatrix, glm::value_ptr(view), sizeof(viewMatrix));
}

void Renderer::updateViewMatrix(const glm::vec3& cameraPos) {
    glm::mat4 view = glm::lookAt(
        cameraPos,
        cameraPos + glm::vec3(0.0f, 0.0f, -1.0f),  // Looking down the negative Z axis
        glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
    );
    memcpy(viewMatrix, glm::value_ptr(view), sizeof(viewMatrix));
}

void Renderer::updateProjectionMatrix() {
    glm::mat4 proj = glm::perspective(
        glm::radians(45.0f),
        (float)width / (float)height,
        0.1f,
        10000.0f
    );
    memcpy(projMatrix, glm::value_ptr(proj), sizeof(projMatrix));
}

void Renderer::clear() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Renderer::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Renderer::pollEvents() {
    glfwPollEvents();
}

void Renderer::swapBuffers() {
    glfwSwapBuffers(window);
} 