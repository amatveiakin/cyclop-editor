#include <QFile>

#include "huge_file_qmodel.h"


huge_file_qmodel::huge_file_qmodel (QObject *parent)
  : QAbstractListModel (parent)
{
}

huge_file_qmodel::~huge_file_qmodel ()
{
}

bool huge_file_qmodel::open_file (QString file_name)
{
  QScopedPointer<QFile> new_file (new QFile (file_name));
  if (!new_file->open (QFile::ReadOnly | QFile::Text))
    return false;

  file.swap (new_file);

  // TODO: optimize
  while (true)
    {
      // warning: may not be equal to the sum of line lengths because of line ends converting
      lines_start_positions.push_back (file->pos ());
      if (file->readLine ().isEmpty ())
        break;
    }
  return true;
}

int huge_file_qmodel::rowCount (const QModelIndex &/*parent*/) const
{
  if (!file)
    return 0;
  return lines_start_positions.size ();
}

QVariant huge_file_qmodel::data (const QModelIndex &index, int role) const
{
  if (role == Qt::DisplayRole && index.row () < lines_start_positions.size ())
    {
      file->seek (lines_start_positions[index.row ()]);
      QByteArray line = file->readLine ();
      line.chop (1);
      return line;
    }
  return QVariant ();
}
