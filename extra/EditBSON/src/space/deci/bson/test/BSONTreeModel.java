package space.deci.bson.test;

import java.util.List;

import de.javagl.treetable.AbstractTreeTableModel;
import de.javagl.treetable.TreeTableModel;
import space.deci.bson.DOMElement;
import space.deci.bson.DataType;

public class BSONTreeModel extends AbstractTreeTableModel {
	
	public BSONTreeModel(DOMElement root) {
		super(root);
	}

	@Override
	public int getColumnCount() {
		return 3;
	}

	@Override
	public String getColumnName(int column) {
		switch(column)
		{
		case 0:
			return "Name";
		case 1:
			return "Type";
		case 2:
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
		DOMElement treeNode = (DOMElement) node;
		
		switch(column)
		{	
		case 0:
			return treeNode.GetKey();
		case 1:
			return treeNode.GetType().toString();
		case 2:			
			switch (treeNode.GetType())
			{
			case NUMBER:
				return treeNode.GetNumber();
			case STRING:
				return treeNode.GetString();
			case DOCUMENT:
				return null;
			default:
				throw new RuntimeException("Unknown Element Type");
			}
		default:
			throw new RuntimeException("Invalid Column");			
		}
	}

	@Override
	public Object getChild(Object parent, int index) {
		DOMElement treeNode = (DOMElement) parent;
		List<DOMElement> itemMap = treeNode.GetDocument();		
		return itemMap.get(index);
	}

	@Override
	public int getChildCount(Object parent) {
		DOMElement treeNode = (DOMElement) parent;
		if (treeNode.GetType() == DataType.DOCUMENT)
		{
			return treeNode.GetDocument().size();		
		}
		else
		{
			return 0;
		}
	}


}
