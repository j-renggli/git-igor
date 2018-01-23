#pragma once

#include <memory>
#include <set>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>

#include <QtWidgets/QDockWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

#include <backend/repository.h>

class QTreeView;

namespace gitigor {

class Backend;
class FileStatus;

class StageModel : public QAbstractItemModel {
  public:
    StageModel(bool index);

    void clear();
    void addFile(const FileStatus& file);
    FileStatus getFile(size_t index) const;
    void update();

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex
    index(int row, int column,
          const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const
        Q_DECL_OVERRIDE;
    int
    rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

  private:
    bool index_;
    std::vector<FileStatus> files_;
};

class UIStaging : public QDockWidget {
    Q_OBJECT

  public:
    UIStaging();
    virtual ~UIStaging();

  public:
    void initialise();

  signals:
    void onShowDiff(const Diff& diff, bool staging);

  private slots:
    void onCommit(bool checked);
    void onUpdate();

    // Work Tree connections
    void onStageFile(const QModelIndex& index);
    void onShowWorkTreeFile(const QModelIndex& index);
    void onUnstageFile(const QModelIndex& index);
    void onShowIndexFile(const QModelIndex& index);

  private:
    bool showFile(size_t index, bool staging);

  private:
    StageModel indexModel_;
    QTreeView* index_;
    StageModel workTreeModel_;
    QTreeView* workTree_;
    QTextEdit* message_;
    QPushButton* commit_;
    QPushButton* pullCommitPush_;
    std::pair<size_t, bool> nowShowing_;
};
}
