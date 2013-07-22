#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui { class MainWindow; }

class QLabel;
class QModelIndex;

class huge_file_qmodel;


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow (QWidget *parent = 0);
  ~MainWindow ();

private:
  QScopedPointer<Ui::MainWindow> ui;
  QScopedPointer<huge_file_qmodel> file_qmodel;
  QLabel *current_line_label;  // parent

private slots:
  void open_file ();
  void save_file ();
  void show_current_line (const QModelIndex &current_index);
};

#endif // MAINWINDOW_H
