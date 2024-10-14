#ifndef FORM_SPECVIEWER2_H
#define FORM_SPECVIEWER2_H

#include <QWidget>

namespace Ui {
class Form_SpecViewer2;
}

class Form_SpecViewer2 : public QWidget
{
    Q_OBJECT

public:
    explicit Form_SpecViewer2(QWidget *parent = nullptr);
    ~Form_SpecViewer2();

private:
    Ui::Form_SpecViewer2 *ui;
};

#endif // FORM_SPECVIEWER2_H
