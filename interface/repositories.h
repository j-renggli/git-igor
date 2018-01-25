#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>

#include <QtWidgets/QDialog>

#include "includes.h"

class QLineEdit;
class QPushButton;
class QTreeView;

namespace gitigor {

class RepositoryManager;
class UIRepoAdd;

class RemotesModel : public QAbstractItemModel {
  public:
    RemotesModel();

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    QModelIndex
    index(int row, int column,
          const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const
        Q_DECL_OVERRIDE;
    int
    rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

  private:
    RepositoryManager& manager_;
};

class interface_EXPORT UIRepositories : public QDialog {
    Q_OBJECT

  public:
    UIRepositories();
    virtual ~UIRepositories();

  public:
    void initialise();

  public slots:
    void onShow();
    void onSelecting(const QModelIndex& index);
    void onCancel();
    void onCommit();
    void onAddRepo();
    void onDeleteRepo();
    void onActivateRepo(const QModelIndex& index);

    void updateUI();

  private:
    size_t selected_;

    QTreeView* repositories_;
    QPushButton* repoDel_;
    QPushButton* repoAdd_;
    QPushButton* repoUp_;
    QPushButton* repoDown_;
    QLineEdit* name_;
    QLineEdit* path_;
    QPushButton* pathButton_;
    RemotesModel remotesModel_;
    QTreeView* remotes_;
    QPushButton* cancel_;
    QPushButton* commit_;

    UIRepoAdd* repoAddUI_;
    UIRepoAdd* remoteAddUI_;
};
}
