#pragma once

// Prevent Qt from including OpenGL headers
#define QT_NO_OPENGL_ES_2
#define QT_NO_OPENGL_ES_3

#include <QMainWindow>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QMap>
#include <QTimer>
#include "World.h"
#include "Simulator.h"

// Forward declarations
class QOpenGLWidget;
class QOpenGLFunctions;
class QOpenGLContext;
class QOpenGLShaderProgram;
class QDockWidget;
class QTreeView;
class QListView;
class QTextEdit;
class QTabWidget;
class QWidget;
class OpenGLWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void updateSimulation();
    void onDockLocationChanged(Qt::DockWidgetArea area);
    void onDockVisibilityChanged(bool visible);
    void resetLayout();

private:
    void setupUI();
    void createDockWidgets();
    void createMenus();
    void createToolBars();
    void storeOriginalDockPosition(QDockWidget* dock, Qt::DockWidgetArea area);

    // Dock widgets for different panels
    QDockWidget* hierarchyDock;
    QDockWidget* projectDock;
    QDockWidget* inspectorDock;
    QTabWidget* viewTabs;

    // Central widget for the main content area
    QWidget* centralWidget;
    OpenGLWidget* glWidget;

    // Store original dock positions
    QMap<QDockWidget*, Qt::DockWidgetArea> originalDockPositions;

    // Timer for simulation updates
    QTimer* simulationTimer;

    // World and simulation
    World m_world;
    Simulator m_simulator;
}; 