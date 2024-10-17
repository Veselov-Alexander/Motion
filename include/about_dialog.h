#pragma once

#include <QDialog>
#include "ui_about_dialog.h"

class AboutDialog : public QDialog, public Ui_Dialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget* parent = 0);
};