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

  bool open_file (QString file_name);

  virtual int rowCount (const QModelIndex &parent = QModelIndex ()) const override;
  virtual QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const override;

signals:
  void set_open_precent (int v);

private:
  QScopedPointer<QFile> file;
  int n_lines;
  QMap<int, qint64> lines_start_positions;
  mutable QCache<int, QString> lines_cache;
};

#endif // HUGE_FILE_QMODEL_H
