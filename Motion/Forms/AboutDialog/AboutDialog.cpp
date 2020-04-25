#include "AboutDialog.h"

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent) 
{
    setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}