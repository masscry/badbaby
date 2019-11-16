package space.deci.bson.test;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;

import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

import space.deci.bson.SAXDocument;

public class ReadBSONFile {
	
	private static BSONTree tree;
	private static JFileChooser fileDialog;
	private static JFrame frame;
	
	
	private static JMenu buildMenu()
	{
		JMenu menu = new JMenu("File");

		JMenuItem exitItem = new JMenuItem("Exit");
		exitItem.addActionListener(new ActionListener() {			
			@Override
			public void actionPerformed(ActionEvent e) {
				System.exit(0);
			}
		});
		
		JMenuItem newItem = new JMenuItem("New");
		newItem.addActionListener(new ActionListener() {			
			@Override
			public void actionPerformed(ActionEvent e) {
				tree.New();
				frame.setTitle("EditBSON");
			}
		});
		
		JMenuItem loadItem = new JMenuItem("Load");
		loadItem.addActionListener(new ActionListener() {			
			@Override
			public void actionPerformed(ActionEvent e) {
				int selectFileResult = fileDialog.showOpenDialog(tree);
				if (selectFileResult == JFileChooser.APPROVE_OPTION)
				{
					File file = fileDialog.getSelectedFile();
					tree.Load(file);
					frame.setTitle(file.getName() + " - EditBSON");
				}
				
			}
		});
		
		JMenuItem saveItem = new JMenuItem("Save");
		saveItem.addActionListener(new ActionListener() {			
			@Override
			public void actionPerformed(ActionEvent e) {
				int selectFileResult = fileDialog.showSaveDialog(tree);
				if (selectFileResult == JFileChooser.APPROVE_OPTION)
				{
					File file = fileDialog.getSelectedFile();					
					SAXDocument saxdoc = new SAXDocument(tree.doc.ToByteArrayAsRoot());
					try
					{
						if (BSONFileFilter.GetExtension(file).equalsIgnoreCase("BSON"))
						{
							saxdoc.Store(file.getCanonicalPath());												
						}
						else
						{
							saxdoc.Store(file.getCanonicalPath() + ".bson");																			
						}
					}
					catch (IOException ioerror)
					{
						JOptionPane.showMessageDialog(null, ioerror.getMessage(), "IOError", JOptionPane.INFORMATION_MESSAGE);
					}
				}
			}
		});
				
		menu.add(newItem);
		menu.add(loadItem);
		menu.add(saveItem);
		menu.addSeparator();
		menu.add(exitItem);
		return menu;
	}
		
	private static void buildUI(String filename)
	{
		try
		{
			UIManager.setLookAndFeel(
				UIManager.getCrossPlatformLookAndFeelClassName()
			);
		}
		catch (Exception error)
		{
			System.err.print("Can't set system look and feel");
			error.printStackTrace();
		}
					
		JMenuBar menuBar = new JMenuBar();
		menuBar.add(buildMenu());
		
		tree = new BSONTree(filename);

		frame = new JFrame("EditBSON");
		if (filename != null)
		{
			frame.setTitle(filename + " - EditBSON");
		}		
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);		
		frame.setMinimumSize(new Dimension(600, 300));		
		frame.add(tree);
		frame.setJMenuBar(menuBar);
		frame.pack();
		frame.setVisible(true);
		
		fileDialog = new JFileChooser();
		fileDialog.addChoosableFileFilter(new BSONFileFilter());
		fileDialog.setAcceptAllFileFilterUsed(false);
	}

	public static void main(String[] args) {
		
		SwingUtilities.invokeLater(new Runnable() {
			@Override
			public void run() {
				if (args.length == 1)
				{
					buildUI(args[0]);
				}
				else
				{
					buildUI(null);					
				}				
			}
		});

	}

}
