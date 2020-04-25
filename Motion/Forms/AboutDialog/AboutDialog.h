#pragma once

#include <QDialog>
#include "ui_AboutDialog.h"

class AboutDialog : public QDialog, public Ui_Dialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget* parent = 0);
};