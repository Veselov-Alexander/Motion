#include "motion/about_dialog.h"

namespace Motion
{

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent) 
{
    setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

}  // namespace Motion