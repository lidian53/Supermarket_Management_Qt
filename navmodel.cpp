
#include "navmodel.h"

#include <QFile>
#include <QDomDocument>

NavModel::NavModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

NavModel::~NavModel()
{
	for ( std::vector<TreeNode*>::iterator it = m_nodeList.begin();
		  it != m_nodeList.end(); )
	{
		for ( std::list<TreeNode*>::iterator child = (*it)->children.begin();
			  child != (*it)->children.end(); )
		{
			delete (*child);
			child = (*it)->children.erase(child);
		}

		delete (*it);
		it = m_nodeList.erase(it);
	}
}

void NavModel::ReadDataFromConfig(QString path)
{
	QFile xml(path);

	if ( !xml.open(QIODevice::ReadOnly | QIODevice::Text) )
        return;

	QDomDocument doc;

	if ( !doc.setContent(&xml, false) )
		return;

	QDomNode root = doc.documentElement().firstChildElement("layout");
	QDomNodeList children = root.childNodes();

	for ( int i = 0; i != children.count(); ++i )
	{
		QDomElement nodeInfo = children.at(i).toElement();
		TreeNode* node = new TreeNode;
		node->label = nodeInfo.attribute("label");
		node->collapse = nodeInfo.attribute("collapse").toInt();
		node->count = nodeInfo.attribute("count").toInt();
		node->level = 1;
        node->isSubItem = false;
		QDomNodeList secondLevel = nodeInfo.childNodes();

        for ( int j = 0; j != secondLevel.count(); ++j )        //子目录
		{
			QDomElement secNodeInfo = secondLevel.at(j).toElement();
			TreeNode* secNode = new TreeNode;
			secNode->label = secNodeInfo.attribute("label");
			secNode->count = secNodeInfo.attribute("count").toInt();
			secNode->collapse = false;
			secNode->level = 2;
            secNode->isSubItem = true;
			secNode->theLast = ( j == secondLevel.count()-1 && i != children.count()-1 );
			node->children.push_back(secNode);
		}

		m_nodeList.push_back(node);
	}
	 
	RefreshList();
	beginInsertRows(QModelIndex(), 0, m_list.size());
	endInsertRows();
}

int NavModel::rowCount( const QModelIndex &parent /* = QModelIndex */ ) const
{
	return m_list.size();
}

QVariant NavModel::data( const QModelIndex &index, int role /* = Qt::DisplayRole */ ) const 
{
	if ( !index.isValid() )
		return QVariant();

	if ( index.row() >= m_list.size() || index.row() < 0 )
		return QVariant();

	if ( role == Qt::DisplayRole ) 
		return m_list[index.row()].label;
	else if ( role == Qt::UserRole )
		return (unsigned int)(m_list[index.row()].treeNode);

	return QVariant();
}

void NavModel::RefreshList()
{
	m_list.clear();

	for ( std::vector<TreeNode*>::iterator it = m_nodeList.begin();
		  it != m_nodeList.end();
		  ++it )
	{
		ListNode node;
		node.label = (*it)->label;
		node.treeNode = *it;

		m_list.push_back(node);
		
		if ( (*it)->collapse )
			continue;

		for ( std::list<TreeNode*>::iterator child = (*it)->children.begin();
			  child != (*it)->children.end();
			  ++child )
		{
			ListNode node;
            node.label = (*child)->label;
			node.treeNode = *child;
			node.treeNode->theLast = false;

			m_list.push_back(node);
		}

		if ( !m_list.empty())
			m_list.back().treeNode->theLast = true;
	}
}

void NavModel::Collapse( const QModelIndex& index )
{
	TreeNode* node = m_list[index.row()].treeNode;
	if ( node->children.size() == 0 )
		return;

	node->collapse = !node->collapse;

	RefreshList();

	if ( !node->collapse )
	{
		beginInsertRows(QModelIndex(), index.row()+1, index.row()+node->children.size());
		endInsertRows();
	}
	else
	{
		beginRemoveRows(QModelIndex(), index.row()+1, index.row()+node->children.size());
		endRemoveRows();
	}

}

void NavModel::clickIndex(const QModelIndex &index)
{
    TreeNode* node = m_list[index.row()].treeNode;
    if(node->isSubItem)
         emit clickItem(index.data().toString());
}

void NavModel::Refresh()
{
	RefreshList();
	beginResetModel();
	endResetModel();
}
