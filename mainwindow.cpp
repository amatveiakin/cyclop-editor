#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>

#include "mainwindow.h"
#include "huge_file_qmodel.h"

#include "ui_mainwindow.h"


const QString app_name= "Cyclop Editor";


MainWindow::MainWindow (QWidget *parent) :
  QMainWindow (parent),
  ui (new Ui::MainWindow)
{
  ui->setupUi (this);

  ui->fileContentsTableView->horizontalHeader ()->setStretchLastSection (true);
  ui->fileContentsTableView->horizontalHeader ()->hide ();
  ui->fileContentsTableView->verticalHeader ()->setDefaultSectionSize (fontMetrics ().height ());
  ui->fileContentsTableView->verticalHeader ()->hide ();
  ui->fileContentsTableView->setShowGrid (false);
  ui->fileContentsTableView->setEditTriggers (QAbstractItemView::AllEditTriggers);

  setWindowTitle (app_name);

  connect (ui->openAction, SIGNAL (triggered ()), this, SLOT (open_file ()));
  connect (ui->saveAction, SIGNAL (triggered ()), this, SLOT (save_file ()));
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
      QMessageBox::warning (this, app_name, "Unable to open file", QMessageBox::Ok);
      return;
    }

  file_qmodel.swap (new_file_qmodel);
  ui->statusBar->showMessage ("Opened successfully");
  setWindowTitle (QString ("%1 - %2").arg (new_file_name, app_name));
  ui->fileContentsTableView->setModel (file_qmodel.data ());
}

void MainWindow::save_file ()
{
  if (!file_qmodel)
    {
      QMessageBox::warning (this, app_name, "Nothing to save", QMessageBox::Ok);
      return;
    }

  QProgressBar file_save_progress_bar;
  ui->statusBar->addWidget (&file_save_progress_bar);
  file_save_progress_bar.setRange (0, 100);
  connect (file_qmodel.data (), SIGNAL (set_save_precent (int)), &file_save_progress_bar, SLOT (setValue (int)));

  bool save_ok = file_qmodel->save_file ();
  if (!save_ok)
    {
      QMessageBox::warning (this, app_name, "Unable to save file", QMessageBox::Ok);
      return;
    }

  ui->statusBar->showMessage ("Saved successfully");
}
