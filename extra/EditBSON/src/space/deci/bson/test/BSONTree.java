package space.deci.bson.test;

import java.awt.GridLayout;
import java.util.Iterator;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreeSelectionModel;

import de.javagl.treetable.JTreeTable;

import space.deci.bson.SAXDocument;
import space.deci.bson.DataType;
import space.deci.bson.Document;
import space.deci.bson.Element;

public class BSONTree extends JPanel {
	
	private static final long serialVersionUID = -1859332234258383841L;

	private JTreeTable tree;

	private void FillTreeNode(Document doc, DefaultMutableTreeNode top)
	{
		Iterator<Element> it = doc.iterator();
		
		while(it.hasNext())
		{
			Element item = it.next();
			if (item.GetType() == DataType.DOCUMENT)
			{
				DefaultMutableTreeNode keyNode = new DefaultMutableTreeNode(item.GetKey());						
				FillTreeNode(item.GetDocument(), keyNode);				
				top.add(keyNode);			
			}
			else
			{
				top.add(new DefaultMutableTreeNode(item));							
			}
		}
	}
		
	public BSONTree(String filename)
	{
		super(new GridLayout(1, 0));
		
		SAXDocument doc = new SAXDocument();
		
		doc.Load(filename);
		
		DefaultMutableTreeNode root = new DefaultMutableTreeNode(filename);
		this.FillTreeNode(doc, root);
		
		this.tree = new JTreeTable(new BSONTreeModel(root));
		this.tree.getSelectionModel().setSelectionMode(
			TreeSelectionModel.SINGLE_TREE_SELECTION
		);
				
		JScrollPane treeScrollPane = new JScrollPane(this.tree);
		
		this.add(treeScrollPane);
	}

}
