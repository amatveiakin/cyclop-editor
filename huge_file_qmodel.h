#ifndef HUGE_FILE_QMODEL_H
#define HUGE_FILE_QMODEL_H

#include <QAbstractListModel>
#include <QCache>
#include <QVector>

class QFile;


class huge_file_qmodel : public QAbstractListModel
{
  Q_OBJECT
public:
  explicit huge_file_qmodel (QObject *parent = 0);
  ~huge_file_qmodel ();

  // WARNING: The open_file function should be called only once after the model is created,
  //          before it is assigned to any views
  // TODO: Do a proper clean-up and allow to call it many times OR check that it is not called twice
  bool open_file (QString file_name);
  bool save_file ();

  virtual int rowCount (const QModelIndex &parent = QModelIndex ()) const override;
  virtual QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const override;
  virtual bool setData (const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
  virtual Qt::ItemFlags flags (const QModelIndex &index) const override;

signals:
  void set_open_precent (int v);
  void set_save_precent (int v);

private:
  QScopedPointer<QFile> file;
  int n_lines;
  QMap<int, qint64> lines_start_positions;
  mutable QCache<int, QString> lines_cache;
  QMap<int, QString> edited_lines;
};

#endif // HUGE_FILE_QMODEL_H
