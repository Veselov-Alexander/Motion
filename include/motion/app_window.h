#pragma once

#include "motion/ui_app_window.h"

#include <QtWidgets/QMainWindow>

namespace Motion
{

class IState;
class Generate;
class AboutDialog;

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    AppWindow(QWidget *parent = Q_NULLPTR);

private slots:
    // File
    void newFile(bool bCheck=true);
    void openFile();
    void saveFile();
    void saveFileAs();
    void exit();

    // Scene
    void createObject();
    void reshapeDevice();
    void moveDevice();
    void undoMove();
    void finish();

    void sceneAction(IState* pIState);

    // View
    void displayMinkowski();
    void displayRoadmap();
    void displayPathInfo();

    // Search Method
    void rapidlyExploringRandomTree();
    void shortestPathRoadmap();
    void voronoiMap();

    // Generate Map
    void generateRandom();
    void generateLabyrinth();
    void generatePolyLabyrinth();

    // Settings
    void useSensors();
    void useSnapping();

    // Help
    void about();

private:
    void setMenuActionsEnabled(QMenu* pMenu, bool bEnabled);
    void setMenuActionsChecked(QMenu* pMenu, bool bChecked);
    void update();
    void generate(Generate* pGenerate);
private:
    bool m_bUnsavedChanges;
    Ui::AppWindowClass m_ui;
    AboutDialog* m_aboutDialog;
};

}  // namespace Motion