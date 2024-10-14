#ifndef FORM_SPECVIEWER3_H
#define FORM_SPECVIEWER3_H

#include <QWidget>

namespace Ui {
class Form_SpecViewer3;
}

class Form_SpecViewer3 : public QWidget
{
    Q_OBJECT

public:
    explicit Form_SpecViewer3(QWidget *parent = nullptr);
    ~Form_SpecViewer3();

private:
    Ui::Form_SpecViewer3 *ui;
};

#endif // FORM_SPECVIEWER3_H
