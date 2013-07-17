#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>

#include "mainwindow.h"
#include "huge_file_qmodel.h"

#include "ui_mainwindow.h"


MainWindow::MainWindow (QWidget *parent) :
  QMainWindow (parent),
  ui (new Ui::MainWindow)
{
  ui->setupUi (this);

  ui->fileContentsTreeView->setRootIsDecorated (false);
  ui->fileContentsTreeView->setItemsExpandable (false);
  ui->fileContentsTreeView->setUniformRowHeights (true);
  ui->fileContentsTreeView->header ()->hide ();

  connect (ui->openAction, SIGNAL (triggered ()), this, SLOT (open_file ()));
  connect (ui->exitAction, SIGNAL (triggered ()), this, SLOT (close ()));
}

MainWindow::~MainWindow ()
{
}

void MainWindow::open_file ()
{
  QString new_file_name = QFileDialog::getOpenFileName (this);
  if (new_file_name.isEmpty ())
    return;

  QScopedPointer<huge_file_qmodel> new_file_qmodel (new huge_file_qmodel ());

  QProgressBar file_open_progress_bar;
  ui->statusBar->addWidget (&file_open_progress_bar);
  file_open_progress_bar.setRange (0, 100);
  connect (new_file_qmodel.data (), SIGNAL (set_open_precent (int)), &file_open_progress_bar, SLOT (setValue (int)));

  if (!new_file_qmodel->open_file (new_file_name))
    {
      QMessageBox::warning (this, "Error", "Unable to open file", QMessageBox::Ok);
      return;
    }

  file_qmodel.swap (new_file_qmodel);
  ui->fileContentsTreeView->setModel (file_qmodel.data ());
}
