package space.deci.bson.test;

import java.awt.Dimension;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

public class ReadBSONFile {
		
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
		
		String filename = args[0];

		JFrame frame = new JFrame(filename);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);		
		frame.setMinimumSize(new Dimension(600, 300));
			
		frame.add(new BSONTree(filename));
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
