#include "motion/app_window.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    srand(time(0));
    QApplication app(argc, argv);
    Motion::AppWindow window;
    window.show();
    return app.exec();
}
