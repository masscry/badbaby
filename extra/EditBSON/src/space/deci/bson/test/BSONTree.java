package space.deci.bson.test;

import java.awt.GridLayout;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.tree.TreeSelectionModel;

import de.javagl.treetable.JTreeTable;
import de.javagl.treetable.TreeTableModel;
import space.deci.bson.SAXParser;
import space.deci.bson.DOMDocument;

public class BSONTree extends JPanel {
	
	private static final long serialVersionUID = -1859332234258383841L;

	private JTreeTable tree;
		
	public BSONTree(String filename)
	{
		super(new GridLayout(1, 0));
		
		SAXParser parser = new SAXParser();
		
		parser.Load(filename);
		
		DOMDocument doc = new DOMDocument(filename, parser);
		
		TreeTableModel model = new BSONTreeModel(doc);
				
		this.tree = new JTreeTable(model);
		this.tree.getSelectionModel().setSelectionMode(
			TreeSelectionModel.SINGLE_TREE_SELECTION
		);
				
		JScrollPane treeScrollPane = new JScrollPane(this.tree);
		
		this.add(treeScrollPane);
	}

}
