package space.deci.bson.test;

import java.util.List;

import de.javagl.treetable.AbstractTreeTableModel;
import de.javagl.treetable.TreeTableModel;
import space.deci.bson.DOMElement;
import space.deci.bson.DOMNumber;
import space.deci.bson.DOMString;
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
	
    @Override
    public boolean isCellEditable(Object node, int column)
    { 
    	if (column == 0)
    	{
    		return getColumnClass(0) == TreeTableModel.class;    		
    	}
    	
		DOMElement treeNode = (DOMElement) node;
		switch(treeNode.GetType())
		{
		case NUMBER:
		case STRING:
			return (column == 2);
		default:
			return false;
		}
    }

    @Override
    public void setValueAt(Object aValue, Object node, int column)
    {
    	if (column != 2)
    	{
    		throw new RuntimeException("Can only change value column");
    	}
		DOMElement treeNode = (DOMElement) node;
		switch(treeNode.GetType())
		{
		case NUMBER:
			DOMNumber num = (DOMNumber) treeNode;
			num.SetValue(Double.parseDouble((String) aValue));
			break;
		case STRING:			
			DOMString str = (DOMString) treeNode;
			str.SetString((String) aValue);
			break;
		default:
			throw new RuntimeException("This tree node type is not editable");
		}
    }
        
}
