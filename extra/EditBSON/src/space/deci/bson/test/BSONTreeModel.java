package space.deci.bson.test;

import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreeNode;

import de.javagl.treetable.AbstractTreeTableModel;
import de.javagl.treetable.TreeTableModel;
import space.deci.bson.Element;

public class BSONTreeModel extends AbstractTreeTableModel {
	
	protected BSONTreeModel(TreeNode root) {
		super(root);
	}

	@Override
	public int getColumnCount() {
		return 2;
	}

	@Override
	public String getColumnName(int column) {
		switch(column)
		{
		case 0:
			return "Elements";
		case 1:
			return "Values";
		default:
			return "???";
		}
	}

	@Override
	public Class<?> getColumnClass(int column) {
        if (column == 0)
        {
            return TreeTableModel.class;                    
        }
        return Object.class;
	}

	@Override
	public Object getValueAt(Object node, int column) {
		DefaultMutableTreeNode treeNode = (DefaultMutableTreeNode) node;
		
		if (treeNode.getChildCount() != 0)
		{
			switch(column)
			{
			case 0:
				return treeNode.getUserObject();
			default:
				return null;
			}
		}
		else
		{
			Element treeElement = (Element) treeNode.getUserObject();
			switch (column)
			{
			case 0:
				return treeElement.GetKey();
			case 1:
				switch(treeElement.GetType())
				{
				case NUMBER:
					return treeElement.GetNumber();
				case STRING:
					return treeElement.GetString();
				default:
					throw new RuntimeException("Unknown TreeElement data type");
				}
			default:
				return null;
			}
		}
	}

	@Override
	public Object getChild(Object parent, int index) {
		TreeNode treeNode = (TreeNode) parent;		
		return treeNode.getChildAt(index);
	}

	@Override
	public int getChildCount(Object parent) {
		TreeNode treeNode = (TreeNode) parent;
		return treeNode.getChildCount();
	}


}
