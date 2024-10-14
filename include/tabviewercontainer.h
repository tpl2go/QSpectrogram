#ifndef TABVIEWERCONTAINER_H
#define TABVIEWERCONTAINER_H

#include <QTabWidget>

namespace Ui {
class TabViewerContainer;
}

class TabViewerContainer : public QTabWidget
{
    Q_OBJECT

public:
    explicit TabViewerContainer(QWidget *parent = nullptr);
    ~TabViewerContainer();

public slots:
    void controller_closetab(int index);
    void make_singleviewertab();
    void make_doubleviewertab();
    void make_tripleviewertab();

private:
    Ui::TabViewerContainer *ui;
    int m_counter = 1;
};

#endif // TABVIEWERCONTAINER_H
