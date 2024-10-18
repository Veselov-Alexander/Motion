#pragma once

#include "motion/ui_about_dialog.h"

#include <QDialog>

namespace Motion
{

class AboutDialog : public QDialog, public Ui_Dialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget* parent = 0);
};

}  // namespace Motion