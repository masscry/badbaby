package space.deci.bson.test;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

public class ReadBSONFile {
	
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
			}
		});
		
		JMenuItem loadItem = new JMenuItem("Load");
		loadItem.addActionListener(new ActionListener() {			
			@Override
			public void actionPerformed(ActionEvent e) {
			}
		});
		
		JMenuItem saveItem = new JMenuItem("Save");
		saveItem.addActionListener(new ActionListener() {			
			@Override
			public void actionPerformed(ActionEvent e) {
			}
		});
				
		menu.add(newItem);
		menu.add(loadItem);
		menu.add(saveItem);
		menu.addSeparator();
		menu.add(exitItem);
		return menu;
	}
		
	private static void buildUI(String[] args)
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

		String filename = args[0];
		
		JFrame frame = new JFrame(filename + " - EditBSON");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);		
		frame.setMinimumSize(new Dimension(600, 300));		
		frame.add(new BSONTree(filename));
		frame.setJMenuBar(menuBar);
		frame.pack();
		frame.setVisible(true);
	}

	public static void main(String[] args) {
		
		if (args.length != 1)
		{
			System.err.print("Usage: ReadBSONFile filename\n\n");
			System.err.print("Basic BSON document editor");
			System.exit(-1);
		}
				
		SwingUtilities.invokeLater(new Runnable() {
			@Override
			public void run() {
				buildUI(args);
			}
		});

	}

}
