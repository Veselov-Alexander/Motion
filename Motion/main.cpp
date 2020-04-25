#include "Forms/AppWindow/AppWindow.h"
#include "QtWidgets/QApplication"


int main(int argc, char *argv[])
{
    srand(time(0));
    QApplication app(argc, argv);
    AppWindow window;
    window.show();
    return app.exec();
}
