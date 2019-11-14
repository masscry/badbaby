package space.deci.bson.test;

import java.awt.GridLayout;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.tree.TreeSelectionModel;

import de.javagl.treetable.JTreeTable;
import de.javagl.treetable.TreeTableModel;
import space.deci.bson.SAXDocument;
import space.deci.bson.DOMDocument;

public class BSONTree extends JPanel {
	
	private static final long serialVersionUID = -1859332234258383841L;

	private JTreeTable tree;
	
	public DOMDocument doc;
		
	public BSONTree(String filename)
	{
		super(new GridLayout(1, 0));
		
		SAXDocument parser = new SAXDocument();
		
		parser.Load(filename);
		
		this.doc = new DOMDocument(filename, parser);
		
		TreeTableModel model = new BSONTreeModel(this.doc);
				
		this.tree = new JTreeTable(model);
		this.tree.getSelectionModel().setSelectionMode(
			TreeSelectionModel.SINGLE_TREE_SELECTION
		);
				
		JScrollPane treeScrollPane = new JScrollPane(this.tree);
		
		this.add(treeScrollPane);
	}

}
