package space.deci.bson.test;

import java.awt.GridLayout;
import java.io.File;
import java.io.IOException;

import javax.swing.JOptionPane;
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
	
	public void New()
	{
		SAXDocument parser = new SAXDocument();
		
		DOMDocument newDoc = new DOMDocument("", parser);
		TreeTableModel model = new BSONTreeModel(newDoc);
		
		this.tree.UpdateTreeTableModel(model);	
		this.doc = newDoc;
	}
	
	public void Load(File file)
	{
		try
		{
			SAXDocument parser = new SAXDocument();
			parser.Load(file.getCanonicalPath());
			
			DOMDocument newDoc = new DOMDocument(file.getName(), parser);
			TreeTableModel model = new BSONTreeModel(newDoc);
		
			this.tree.UpdateTreeTableModel(model);
			this.doc = newDoc;
		}
		catch (IOException ioerror)
		{
			JOptionPane.showMessageDialog(null, ioerror.getMessage(), "IOError", JOptionPane.INFORMATION_MESSAGE);			
		}
	}
			
	public BSONTree(String filename)
	{
		super(new GridLayout(1, 0));
		
		SAXDocument parser = new SAXDocument();
		
		if (filename != null)
		{
			parser.Load(filename);
			this.doc = new DOMDocument(filename, parser);
		}
		else
		{
			this.doc = new DOMDocument("", parser);
		}
		
		TreeTableModel model = new BSONTreeModel(this.doc);

		this.tree = new JTreeTable(model);
		this.tree.getSelectionModel().setSelectionMode(
			TreeSelectionModel.SINGLE_TREE_SELECTION
		);

		JScrollPane treeScrollPane = new JScrollPane(this.tree);
		
		this.add(treeScrollPane);
	}

}
