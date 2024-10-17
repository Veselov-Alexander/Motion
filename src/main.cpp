#include "AppWindow.h"
#include "QtWidgets/QApplication"
#include <omp.h>

int main(int argc, char* argv[])
{
    omp_set_num_threads(8);
    srand(time(0));
    QApplication app(argc, argv);
    AppWindow window;
    window.show();
    return app.exec();
}
