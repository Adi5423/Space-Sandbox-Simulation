#include "MainWindow.h"
#include <QVBoxLayout>
#include <QTreeView>
#include <QListView>
#include <QTextEdit>
#include <QTabWidget>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include "OpenGLWidget.h"
#include "World.h"
#include "Simulator.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , hierarchyDock(nullptr)
    , projectDock(nullptr)
    , inspectorDock(nullptr)
    , viewTabs(nullptr)
    , centralWidget(nullptr)
    , glWidget(nullptr)
    , simulationTimer(nullptr)
    , m_world()
    , m_simulator(m_world)
{
    setupUI();

    // Create and start simulation timer
    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &MainWindow::updateSimulation);
    simulationTimer->start(16); // ~60 FPS
}

void MainWindow::setupUI()
{
    // Set window properties
    setWindowTitle("Space Sandbox Simulation");
    resize(1280, 720);

    // Create central widget
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create dock widgets
    createDockWidgets();
    
    // Create menus and toolbars
    createMenus();
    createToolBars();
}

void MainWindow::createDockWidgets()
{
    // Hierarchy Panel (Left)
    hierarchyDock = new QDockWidget("Hierarchy", this);
    hierarchyDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    hierarchyDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    QTreeView* hierarchyView = new QTreeView(hierarchyDock);
    hierarchyDock->setWidget(hierarchyView);
    addDockWidget(Qt::LeftDockWidgetArea, hierarchyDock);
    storeOriginalDockPosition(hierarchyDock, Qt::LeftDockWidgetArea);
    connect(hierarchyDock, &QDockWidget::dockLocationChanged, this, &MainWindow::onDockLocationChanged);
    connect(hierarchyDock, &QDockWidget::visibilityChanged, this, &MainWindow::onDockVisibilityChanged);

    // Project Panel (Bottom)
    projectDock = new QDockWidget("Project", this);
    projectDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    projectDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    QListView* projectView = new QListView(projectDock);
    projectDock->setWidget(projectView);
    addDockWidget(Qt::BottomDockWidgetArea, projectDock);
    storeOriginalDockPosition(projectDock, Qt::BottomDockWidgetArea);
    connect(projectDock, &QDockWidget::dockLocationChanged, this, &MainWindow::onDockLocationChanged);
    connect(projectDock, &QDockWidget::visibilityChanged, this, &MainWindow::onDockVisibilityChanged);

    // Inspector Panel (Right)
    inspectorDock = new QDockWidget("Inspector", this);
    inspectorDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    inspectorDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    QTextEdit* inspectorView = new QTextEdit(inspectorDock);
    inspectorDock->setWidget(inspectorView);
    addDockWidget(Qt::RightDockWidgetArea, inspectorDock);
    storeOriginalDockPosition(inspectorDock, Qt::RightDockWidgetArea);
    connect(inspectorDock, &QDockWidget::dockLocationChanged, this, &MainWindow::onDockLocationChanged);
    connect(inspectorDock, &QDockWidget::visibilityChanged, this, &MainWindow::onDockVisibilityChanged);

    // Create a central widget for the tabbed view
    QWidget* centralTabWidget = new QWidget(this);
    QVBoxLayout* centralLayout = new QVBoxLayout(centralTabWidget);
    
    // Create tab widget for Game and Viewport
    viewTabs = new QTabWidget(centralTabWidget);
    viewTabs->setTabPosition(QTabWidget::North);
    
    // Viewport View
    glWidget = new OpenGLWidget(m_world, m_simulator, viewTabs);
    viewTabs->addTab(glWidget, "Viewport");

    // Game View
    QWidget* gameView = new QWidget(viewTabs);
    gameView->setStyleSheet("background-color: #2D2D2D;");
    viewTabs->addTab(gameView, "Game");
    
    centralLayout->addWidget(viewTabs);
    setCentralWidget(centralTabWidget);
}

void MainWindow::storeOriginalDockPosition(QDockWidget* dock, Qt::DockWidgetArea area)
{
    originalDockPositions[dock] = area;
}

void MainWindow::onDockLocationChanged(Qt::DockWidgetArea area)
{
    QDockWidget* dock = qobject_cast<QDockWidget*>(sender());
    if (dock && area != Qt::NoDockWidgetArea) {
        originalDockPositions[dock] = area;
    }
}

void MainWindow::onDockVisibilityChanged(bool visible)
{
    QDockWidget* dock = qobject_cast<QDockWidget*>(sender());
    if (dock && visible && dock->isFloating()) {
        Qt::DockWidgetArea area = dockWidgetArea(dock);
        if (area != Qt::NoDockWidgetArea) {
            originalDockPositions[dock] = area;
        }
    }
}

void MainWindow::resetLayout()
{
    removeDockWidget(hierarchyDock);
    removeDockWidget(projectDock);
    removeDockWidget(inspectorDock);

    if (originalDockPositions.contains(hierarchyDock)) {
        addDockWidget(originalDockPositions[hierarchyDock], hierarchyDock);
        hierarchyDock->setFloating(false);
        hierarchyDock->show();
    }

    if (originalDockPositions.contains(projectDock)) {
        addDockWidget(originalDockPositions[projectDock], projectDock);
        projectDock->setFloating(false);
        projectDock->show();
    }

    if (originalDockPositions.contains(inspectorDock)) {
        addDockWidget(originalDockPositions[inspectorDock], inspectorDock);
        inspectorDock->setFloating(false);
        inspectorDock->show();
    }
}

void MainWindow::updateSimulation()
{
    // Update the simulation
    m_simulator.step(16.0f / 1000.0f); // Convert 16ms to seconds
    
    // Request a redraw of the OpenGL widget
    if (glWidget) {
        glWidget->update();
    }
}

void MainWindow::createMenus()
{
    // File Menu
    QMenu* fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("New Scene");
    fileMenu->addAction("Open Scene");
    fileMenu->addAction("Save Scene");
    fileMenu->addSeparator();
    fileMenu->addAction("Exit");

    // Edit Menu
    QMenu* editMenu = menuBar()->addMenu("Edit");
    editMenu->addAction("Undo");
    editMenu->addAction("Redo");
    editMenu->addSeparator();
    editMenu->addAction("Cut");
    editMenu->addAction("Copy");
    editMenu->addAction("Paste");

    // Window Menu
    QMenu* windowMenu = menuBar()->addMenu("Window");
    QAction* resetLayoutAction = windowMenu->addAction("Reset Layout");
    connect(resetLayoutAction, &QAction::triggered, this, &MainWindow::resetLayout);
    windowMenu->addSeparator();
    windowMenu->addAction("Next Window");
    windowMenu->addAction("Previous Window");
}

void MainWindow::createToolBars()
{
    // Main Toolbar
    QToolBar* mainToolBar = addToolBar("Main Toolbar");
    mainToolBar->addAction("File");
    mainToolBar->addAction("Edit");
    mainToolBar->addAction("Assets");
    mainToolBar->addAction("GameObject");
    mainToolBar->addAction("Component");
    mainToolBar->addAction("Window");
    mainToolBar->addAction("Help");
}

MainWindow::~MainWindow() {} 