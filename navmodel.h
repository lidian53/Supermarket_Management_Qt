#ifndef NAVMODEL_H
#define NAVMODEL_H

#include <QAbstractListModel>
#include <vector>

class NavModel : public QAbstractListModel
{
	Q_OBJECT


public:

	struct TreeNode
	{
		QString label;
		int level;
		bool collapse;
		bool theLast;
		int count;
		std::list<TreeNode*> children;
        bool isSubItem;
	};

	struct ListNode
	{
		QString label;
		TreeNode* treeNode;
	};

	NavModel(QObject *parent);
	~NavModel();

	/*!
	\brief
		Read data from xml.
	*/
	void ReadDataFromConfig(QString path);

	void Refresh();

public:
	int rowCount(const QModelIndex &parent /* = QModelIndex */) const;
	QVariant data(const QModelIndex &index, int role /* = Qt::DisplayRole */) const;

public slots:
	void Collapse(const QModelIndex& index);
    void clickIndex(const QModelIndex &index);
	
private:
	void RefreshList();
signals:
    void clickItem(QString text);

private:
	std::vector<TreeNode*> m_nodeList;
	std::vector<ListNode> m_list;
};

#endif // NAVMODEL_H
