#pragma once

// Prevent Qt from including OpenGL headers
#define QT_NO_OPENGL_ES_2
#define QT_NO_OPENGL_ES_3

// Include GLAD first
#include <glad/glad.h>

// Now include Qt OpenGL headers
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QMatrix4x4>
#include <glm/glm.hpp>
#include "World.h"
#include "Simulator.h"
#include "Mesh.h"

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    OpenGLWidget(World& world, Simulator& simulator, QWidget* parent = nullptr);
    ~OpenGLWidget();

    void setCameraPosition(const glm::vec3& pos);
    void setCameraFront(const glm::vec3& front);
    void setCameraUp(const glm::vec3& up);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void setupMeshBuffers(const std::string& name, const Mesh& mesh);
    bool initializeGLAD();

    // World and simulation
    World& m_world;
    Simulator& m_simulator;

    // OpenGL context and program
    QOpenGLContext* m_context;
    QOpenGLShaderProgram* m_program;

    // Camera properties
    glm::vec3 m_cameraPos;
    glm::vec3 m_cameraFront;
    glm::vec3 m_cameraUp;
    glm::mat4 m_projectionMatrix;

    // Shader uniform locations
    int m_modelLoc;
    int m_viewLoc;
    int m_projLoc;
    int m_colorLoc;
    int m_lightPosLoc;
    int m_viewPosLoc;

    // Mesh data
    struct MeshOpenGLData {
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        GLsizei indexCount;
    };
    std::map<std::string, MeshOpenGLData> m_meshOpenGLData;
}; 