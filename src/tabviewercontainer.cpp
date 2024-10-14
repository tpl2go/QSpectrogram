#include "tabviewercontainer.h"
#include "ui_tabviewercontainer.h"
#include "form_specviewer1.h"
#include "form_specviewer2.h"
#include "form_specviewer3.h"
#include "fmt/core.h"
#include <iostream>

TabViewerContainer::TabViewerContainer(QWidget *parent)
    : QTabWidget(parent)
    , ui(new Ui::TabViewerContainer)
{
    ui->setupUi(this);

    QObject::connect(this, &QTabWidget::tabCloseRequested,
                     this, &TabViewerContainer::controller_closetab);

    make_singleviewertab();
}

TabViewerContainer::~TabViewerContainer()
{
    delete ui;
}

void TabViewerContainer::controller_closetab(int index)
{

    auto w = widget(index);
    removeTab(index);
    delete w;
    if (count() == 0)
    {
        make_singleviewertab();
    }
}


void TabViewerContainer::make_singleviewertab()
{
    auto form = new Form_SpecViewer1(this);
    std::string name = fmt::format("Viewer {}", m_counter);
    addTab(form, QString::fromStdString(name));
    m_counter += 1;
    setCurrentWidget(form);
}

void TabViewerContainer::make_doubleviewertab()
{
    auto form = new Form_SpecViewer2(this);
    std::string name = fmt::format("Viewer {}", m_counter);
    addTab(form, QString::fromStdString(name));
    m_counter += 1;
    setCurrentWidget(form);
}


void TabViewerContainer::make_tripleviewertab()
{
    auto form = new Form_SpecViewer3(this);
    std::string name = fmt::format("Viewer {}", m_counter);
    addTab(form, QString::fromStdString(name));
    m_counter += 1;
    setCurrentWidget(form);
}

