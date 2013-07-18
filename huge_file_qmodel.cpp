#include <QFile>
#include <QTemporaryFile>

#include "huge_file_qmodel.h"


static const int default_line_markers_gap = 200;
static const int rows_seen_at_once_expected_count = 80;


// TODO: Is the `QFile::Text' flag necessary?

huge_file_qmodel::huge_file_qmodel (QObject *parent)
  : QAbstractListModel (parent)
  , n_lines (0)
  , lines_cache ((default_line_markers_gap + rows_seen_at_once_expected_count) * 2)
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
  double progress_denominator = 100. / file->size ();

  // TODO: optimize
  n_lines = 0;
  do
    {
      // warning: may not be equal to the sum of line lengths because of line ends converting
      qint64 file_pos = file->pos ();
      if (n_lines % default_line_markers_gap == 0)
        lines_start_positions[n_lines] = file_pos;
      emit set_open_precent ((int)(file_pos * progress_denominator));
      ++n_lines;
    }
  while (!file->readLine ().isEmpty ());
  --n_lines;
  return true;
}

bool huge_file_qmodel::save_file ()
{
  // TODO: check file functions return values
  if (!file)
    return false;

  double progress_denominator = 100. / n_lines;

  QString target_file_name = file->fileName ();
  QTemporaryFile tmp_file (target_file_name + ".XXXXXX");
  tmp_file.open ();
  QMap<int, qint64> new_lines_start_positions;
  for (int i_line = 0; i_line < n_lines; ++i_line)
    {
      if (i_line % default_line_markers_gap == 0)
        new_lines_start_positions[i_line] = tmp_file.pos ();
      tmp_file.write (data (index (i_line)).toByteArray () + '\n');
      emit set_save_precent ((int)(i_line * progress_denominator));
    }

  file->remove ();
  tmp_file.setAutoRemove (false);
  tmp_file.rename (target_file_name);

  lines_start_positions = new_lines_start_positions;
  lines_cache.clear ();
  edited_lines.clear ();
  file.reset (new QFile (target_file_name));
  file->open (QFile::ReadOnly | QFile::Text);

  return true;
}

int huge_file_qmodel::rowCount (const QModelIndex &/*parent*/) const
{
  return n_lines;
}

QVariant huge_file_qmodel::data (const QModelIndex &index, int role) const
{
  int requested_line = index.row ();
  if ((role == Qt::DisplayRole || role == Qt::EditRole) && requested_line < n_lines)
    {
      if (edited_lines.contains (requested_line))
        return edited_lines[requested_line];
      if (!lines_cache.contains (requested_line))
        {
          auto lines_end_it = lines_start_positions.upperBound (requested_line);
          auto lines_begin_it = lines_end_it - 1;
          int lines_begin = lines_begin_it.key ();
          int lines_end = (lines_end_it == lines_start_positions.end ()) ? n_lines : lines_end_it.key ();
          qDebug ("seeking to %lld, reading lines %d..%d", lines_begin_it.value (), lines_begin, lines_end);
          file->seek (lines_begin_it.value ());
          for (int file_line = lines_begin; file_line < lines_end; ++file_line)
            {
              QByteArray line_data = file->readLine ();
              Q_ASSERT (!line_data.isEmpty ());
              if (line_data.endsWith ('\n'))
                line_data.chop (1);
              lines_cache.insert (file_line, new QString (line_data));
            }
        }
      return *lines_cache[requested_line];
    }
  return QVariant ();
}

bool huge_file_qmodel::setData (const QModelIndex &index, const QVariant &value, int role)
{
  int i_line = index.row ();
  if (role == Qt::EditRole && i_line < n_lines)
    {
      edited_lines[i_line] = value.toString ();
      return true;
    }
  return false;
}

Qt::ItemFlags huge_file_qmodel::flags (const QModelIndex &/*index*/) const
{
  return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}
