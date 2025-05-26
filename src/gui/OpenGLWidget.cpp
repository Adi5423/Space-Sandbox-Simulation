#include <glad/glad.h>  // Include GLAD first
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include "OpenGLWidget.h"
#include "World.h"
#include "Simulator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

OpenGLWidget::OpenGLWidget(World& world, Simulator& simulator, QWidget* parent)
    : QOpenGLWidget(parent)
    , m_world(world)
    , m_simulator(simulator)
    , m_context(nullptr)
    , m_program(nullptr)
    , m_cameraPos(0.0f, 5.0f, 10.0f)
    , m_cameraFront(0.0f, 0.0f, -1.0f)
    , m_cameraUp(0.0f, 1.0f, 0.0f)
    , m_projectionMatrix(glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f))
    , m_modelLoc(-1)
    , m_viewLoc(-1)
    , m_projLoc(-1)
    , m_colorLoc(-1)
    , m_lightPosLoc(-1)
    , m_viewPosLoc(-1)
{
    // Set the focus policy to accept key events
    setFocusPolicy(Qt::StrongFocus);
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();
    // Clean up OpenGL resources
    if (m_program) delete m_program;
    // Clean up mesh buffers (VAOs, VBOs)
    for (auto const& [name, meshData] : m_meshOpenGLData) {
        glDeleteVertexArrays(1, &meshData.vao);
        glDeleteBuffers(1, &meshData.vbo);
        glDeleteBuffers(1, &meshData.ebo);
    }
    doneCurrent();
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // Initialize GLAD
    if (!initializeGLAD()) {
        qDebug() << "Failed to initialize GLAD";
        return;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Enable multisampling for anti-aliasing
    glEnable(GL_MULTISAMPLE);

    // Create shader program
    m_program = new QOpenGLShaderProgram(this);

    const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

    const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 color;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * color;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * color;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
)";

    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
        qDebug() << "Vertex shader compile error:" << m_program->log();
        return;
    }

    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)) {
        qDebug() << "Fragment shader compile error:" << m_program->log();
        return;
    }

    if (!m_program->link()) {
        qDebug() << "Shader program link error:" << m_program->log();
        return;
    }

    // Get uniform locations
    m_modelLoc = m_program->uniformLocation("model");
    m_viewLoc = m_program->uniformLocation("view");
    m_projLoc = m_program->uniformLocation("projection");
    m_colorLoc = m_program->uniformLocation("color");
    m_lightPosLoc = m_program->uniformLocation("lightPos");
    m_viewPosLoc = m_program->uniformLocation("viewPos");

    // Setup mesh buffers for existing meshes
    const Mesh* earthMesh = m_world.getMesh("earth");
    if (earthMesh) {
        setupMeshBuffers("earth", *earthMesh);
    }
    
    const Mesh* moonMesh = m_world.getMesh("moon");
    if (moonMesh) {
        setupMeshBuffers("moon", *moonMesh);
    }
}

bool OpenGLWidget::initializeGLAD()
{
    // Get the OpenGL context from Qt
    QOpenGLContext* context = QOpenGLContext::currentContext();
    if (!context) {
        qDebug() << "No OpenGL context available";
        return false;
    }

    // Get the function pointer loader from Qt
    auto getProcAddress = [](const char* name) -> void* {
        return (void*)QOpenGLContext::currentContext()->getProcAddress(name);
    };

    // Initialize GLAD
    return gladLoadGLLoader((GLADloadproc)getProcAddress);
}

void OpenGLWidget::setupMeshBuffers(const std::string& name, const Mesh& mesh) {
    GLuint vao, vbo, ebo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    // Get vertices and convert to OpenGL format
    const auto& vertices = mesh.getVertices();
    std::vector<float> vertexData;
    for (const auto& vertex : vertices) {
        // Position
        vertexData.push_back(vertex.position.x);
        vertexData.push_back(vertex.position.y);
        vertexData.push_back(vertex.position.z);
        // Normal
        vertexData.push_back(vertex.normal.x);
        vertexData.push_back(vertex.normal.y);
        vertexData.push_back(vertex.normal.z);
        // TexCoord
        vertexData.push_back(vertex.texCoord.x);
        vertexData.push_back(vertex.texCoord.y);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // Get triangles and convert to indices
    const auto& triangles = mesh.getTriangles();
    std::vector<unsigned int> indices;
    for (const auto& triangle : triangles) {
        indices.push_back(triangle.v1);
        indices.push_back(triangle.v2);
        indices.push_back(triangle.v3);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // Vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // Vertex Texture Coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    MeshOpenGLData data;
    data.vao = vao;
    data.vbo = vbo;
    data.ebo = ebo;
    data.indexCount = static_cast<GLsizei>(indices.size());
    m_meshOpenGLData[name] = data;
}

void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    // Update projection matrix on resize
    m_projectionMatrix = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 1000.0f);
}

void OpenGLWidget::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use shader program
    if (m_program && m_program->bind()) {
        // Set view and projection matrices
        glm::mat4 view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
        
        // Convert GLM matrices to QMatrix4x4
        QMatrix4x4 viewMatrix;
        QMatrix4x4 projMatrix;
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                viewMatrix(i, j) = view[i][j];
                projMatrix(i, j) = m_projectionMatrix[i][j];
            }
        }
        
        m_program->setUniformValue("view", viewMatrix);
        m_program->setUniformValue("projection", projMatrix);
        
        // Convert GLM vec3 to QVector3D for viewPos
        QVector3D viewPosVec(m_cameraPos.x, m_cameraPos.y, m_cameraPos.z);
        m_program->setUniformValue("viewPos", viewPosVec);

        // Set light position (example light position)
        QVector3D lightPosVec(10.0f, 10.0f, 10.0f);
        m_program->setUniformValue("lightPos", lightPosVec);

        // Render each body with its mesh
        for (size_t i = 0; i < m_world.getBodyCount(); ++i) {
            const Body& body = m_world.getBody(i);
            std::string meshName = (i == 0) ? "earth" : "moon";
            auto it = m_meshOpenGLData.find(meshName);

            if (it != m_meshOpenGLData.end()) {
                const MeshOpenGLData& meshData = it->second;

                // Create model matrix
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(body.position.x, body.position.y, body.position.z));

                // Convert GLM model matrix to QMatrix4x4
                QMatrix4x4 modelMatrix;
                for(int i = 0; i < 4; i++) {
                    for(int j = 0; j < 4; j++) {
                        modelMatrix(i, j) = model[i][j];
                    }
                }
                m_program->setUniformValue("model", modelMatrix);

                QVector3D colorVec(1.0f, 1.0f, 1.0f);
                m_program->setUniformValue("color", colorVec);

                // Draw mesh
                glBindVertexArray(meshData.vao);
                glDrawElements(GL_TRIANGLES, meshData.indexCount, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            } else {
                qDebug() << "Mesh data not found for:" << QString::fromStdString(meshName);
            }
        }

        m_program->release();
    }

    // Render the grid (adapt from your Renderer class)
    // You'll need access to your Grid class and its rendering logic
    // For example:
    // grid->render(view, projection, cameraPos);
}

void OpenGLWidget::setCameraPosition(const glm::vec3& pos)
{
    m_cameraPos = pos;
}

void OpenGLWidget::setCameraFront(const glm::vec3& front)
{
    m_cameraFront = front;
}

void OpenGLWidget::setCameraUp(const glm::vec3& up)
{
    m_cameraUp = up;
} 