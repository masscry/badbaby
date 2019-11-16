package space.deci.bson.test;

import java.io.File;

import javax.swing.filechooser.FileFilter;

public class BSONFileFilter extends FileFilter {
	
	public static String GetExtension(File file)
	{
		String name = file.getName();
		int lastDot = name.lastIndexOf('.');
		if (lastDot == -1)
		{
			return "";
		}			
		return name.substring(lastDot + 1);
	}
	
	@Override
	public boolean accept(File file) {
		if (file.isDirectory())
		{
			return true;
		}
		
		return GetExtension(file).equalsIgnoreCase("BSON");		
	}

	@Override
	public String getDescription() {
		return "BSON Document";
	}

}
