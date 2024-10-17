#include "app_window.h"

#include "display_view.h"
#include "states/idle_state.h"
#include "states/create_state.h"
#include "states/reshape_state.h"
#include "states/drag_state.h"
#include "file_manager.h"
#include "algorithms/find_methods/rrt.h"
#include "algorithms/find_methods/visibility_graph.h"
#include "algorithms/find_methods/voronoi_map.h"
#include "generate/generate_random.h"
#include "generate/generate_labyrinth.h"
#include "generate/generate_poly_labyrinth.h"

#include "QFileDialog"


AppWindow::AppWindow(QWidget *parent) : QMainWindow(parent)
{
    m_ui.setupUi(this);

    // File
    connect(m_ui.actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile())); 
    connect(m_ui.actionSave_as, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    connect(m_ui.actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(m_ui.actionExit, SIGNAL(triggered()), this, SLOT(exit()));

    // Scene
    connect(m_ui.actionReshape_Device, SIGNAL(triggered()), this, SLOT(reshapeDevice()));
    connect(m_ui.actionCreate_Object, SIGNAL(triggered()), this, SLOT(createObject()));
    connect(m_ui.actionMove_Device, SIGNAL(triggered()), this, SLOT(moveDevice()));
    connect(m_ui.actionUndo_move, SIGNAL(triggered()), this, SLOT(undoMove()));
    connect(m_ui.actionFinish, SIGNAL(triggered()), this, SLOT(finish()));

    // View
    connect(m_ui.actionDisplay_Minkowski, SIGNAL(triggered()), this, SLOT(displayMinkowski()));
    connect(m_ui.actionDisplay_Map, SIGNAL(triggered()), this, SLOT(displayRoadmap()));
    connect(m_ui.actionDisplay_path_info, SIGNAL(triggered()), this, SLOT(displayPathInfo()));
    
    // Search Method
    connect(m_ui.actionRapidly_exploring_random_tree, SIGNAL(triggered()), this, SLOT(rapidlyExploringRandomTree()));
    connect(m_ui.actionShortest_path_roadmap, SIGNAL(triggered()), this, SLOT(shortestPathRoadmap()));
    connect(m_ui.actionVoronoi_map, SIGNAL(triggered()), this, SLOT(voronoiMap()));

    // Generate Map
    connect(m_ui.actionRandom, SIGNAL(triggered()), this, SLOT(generateRandom()));
    connect(m_ui.actionLabyrinth, SIGNAL(triggered()), this, SLOT(generateLabyrinth()));
    connect(m_ui.actionPolyLabyrinth, SIGNAL(triggered()), this, SLOT(generatePolyLabyrinth()));

    // Settings
    connect(m_ui.actionUsing_sensors, SIGNAL(triggered()), this, SLOT(useSensors()));
    connect(m_ui.actionSnapping, SIGNAL(triggered()), this, SLOT(useSnapping()));

    // Help
    connect(m_ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    
    m_bUnsavedChanges = true;
    update();

    m_aboutDialog = new AboutDialog(this);
}

void AppWindow::setMenuActionsEnabled(QMenu* pMenu, bool bEnabled)
{
    for (QAction* action : pMenu->actions())
    {
        action->setEnabled(bEnabled);
    }
}

void AppWindow::setMenuActionsChecked(QMenu* pMenu, bool bChecked)
{
    for (QAction* action : pMenu->actions())
    {
        action->setChecked(bChecked);
    }
}

void AppWindow::createObject()
{
    sceneAction(new CreateState());
}

void AppWindow::reshapeDevice()
{
    sceneAction(new ReshapeState());
}

void AppWindow::moveDevice()
{
    sceneAction(new DragState());
}

void AppWindow::undoMove()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    pDisplayView->undoDeviceMove();
}

void AppWindow::sceneAction(IState* pIState)
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    pDisplayView->setState(pIState);

    setMenuActionsEnabled(m_ui.menuObject, false);
    setMenuActionsEnabled(m_ui.menuFile, false);
    m_ui.actionFinish->setEnabled(true);
}

void AppWindow::finish()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    IState* pCurrentState = pDisplayView->getState();
    assert(pCurrentState);

    pCurrentState->finish();

    pDisplayView->setState(new IdleState());

    setMenuActionsEnabled(m_ui.menuObject, true);
    setMenuActionsEnabled(m_ui.menuFile, true);
    m_ui.actionFinish->setEnabled(false);
    m_bUnsavedChanges = true;
    update();
}

void AppWindow::displayMinkowski()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    pDisplayView->displayMinkowski(m_ui.actionDisplay_Minkowski->isChecked());
}

void AppWindow::displayRoadmap()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    pDisplayView->displayRoadmap(m_ui.actionDisplay_Map->isChecked());
}

void AppWindow::displayPathInfo()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    pDisplayView->displayPathInfo(m_ui.actionDisplay_path_info->isChecked());
}

void AppWindow::rapidlyExploringRandomTree()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    setMenuActionsChecked(m_ui.menuMethod, false);
    m_ui.actionRapidly_exploring_random_tree->setChecked(true);
    pDisplayView->setFindMethod(new RRT(DisplayView::WIDTH, DisplayView::HEIGHT));
}

void AppWindow::shortestPathRoadmap()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    setMenuActionsChecked(m_ui.menuMethod, false);
    m_ui.actionShortest_path_roadmap->setChecked(true);
    pDisplayView->setFindMethod(new VisibilityGraph());
}

void AppWindow::voronoiMap()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    setMenuActionsChecked(m_ui.menuMethod, false);
    m_ui.actionVoronoi_map->setChecked(true);
    pDisplayView->setFindMethod(new VoronoiMap(DisplayView::WIDTH, DisplayView::HEIGHT));
}

void AppWindow::generate(Generate* pGenerate)
{
    if (FileManager::checkNeedToSave())
    {
        saveFileAs();
        return;
    }

    newFile(false);

    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    pGenerate->initialize(pDisplayView);
    pGenerate->generate();
}

void AppWindow::generateRandom()
{
    generate(new GenerateRandom());
}

void AppWindow::generateLabyrinth()
{
    generate(new GenerateLabyrinth());
}

void AppWindow::generatePolyLabyrinth()
{
    generate(new GeneratePolyLabyrinth());
}

void AppWindow::useSensors()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    pDisplayView->useSensors(m_ui.actionUsing_sensors->isChecked());
}

void AppWindow::useSnapping()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);
    pDisplayView->setUseSnapping(m_ui.actionSnapping->isChecked());
}

void AppWindow::about()
{
    m_aboutDialog->show();
}

void AppWindow::newFile(bool bCheck)
{
    if (bCheck && FileManager::checkNeedToSave())
    {
        saveFileAs();
        return;
    }

    FileManager::newFile();
    m_bUnsavedChanges = true;
    update();
}

void AppWindow::openFile()
{
    if (FileManager::checkNeedToSave())
    {
        saveFileAs();
        return;
    }

    QString path = QFileDialog::getOpenFileName(
        this,
        "Open...",
        "",
        "Motion files (*.mo)");

    if (path != "")
    {
        FileManager::openFile(path);
        update();
    }
}

void AppWindow::saveFileAs()
{
    QString path = QFileDialog::getSaveFileName(
        this,
        "Save as...", 
        FileManager::getFilename(),
        "Motion files (*.mo)");

    if (path != "")
    {
        FileManager::saveFile(path);
        m_bUnsavedChanges = false;
        update();
    }
}

void AppWindow::saveFile()
{
    QString path = FileManager::getFilename();

    if (path != UNTITLED_FILENAME)
    {
        FileManager::saveFile(path);
        m_bUnsavedChanges = false;
        update();
    }
    else
    {
        AppWindow::saveFileAs();
    }
}

void AppWindow::exit()
{
    qApp->exit();
}

void AppWindow::update()
{
    QString filepath = FileManager::getFilename();
    setWindowTitle(QString("Motion - ") + (m_bUnsavedChanges ? "*" : "") + filepath);

    displayMinkowski();
    displayRoadmap();
    displayPathInfo();

    if (m_ui.actionRapidly_exploring_random_tree->isChecked())
    {
        rapidlyExploringRandomTree();
    }
    else if (m_ui.actionShortest_path_roadmap->isChecked())
    {
        shortestPathRoadmap();
    }
    else if (m_ui.actionVoronoi_map->isChecked())
    {
        voronoiMap();
    }
}