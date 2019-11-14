package space.deci.bson;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Iterator;

public final class SAXDocument implements Document {
	
	private byte[] data;

	@Override
	public void Load(String filename) {
		try(LEDataInputStream input = new LEDataInputStream(new BufferedInputStream(new FileInputStream(filename))))
		{
			int docSize = input.readInt();
			byte[] tempData = new byte[docSize-4]; // Whole document - size of (docSize)
			
			if (input.read(tempData) != docSize-4)
			{
				return;
			}
			this.data = tempData;
		}
		catch(FileNotFoundException notFound)
		{
			System.err.println("File \"" + filename + "\" not found");
			notFound.printStackTrace();
		}
		catch(IOException ioexcept)
		{
			System.err.println("IOException on loading \"" + filename + "\"");
		}
	}
	
	@Override
	public void Save(String filename) {
		try(LEDataOutputStream output = new LEDataOutputStream(new BufferedOutputStream(new FileOutputStream(filename))))
		{
			int docSize = this.data.length + 4;
			
			output.write(docSize);
			output.write(this.data);			
		}
		catch (FileNotFoundException notFound)
		{
			System.err.println("File \"" + filename + "\" not found");
			notFound.printStackTrace();			
		}
		catch (IOException ioexcept)
		{
			System.err.println("IOException on storing \"" + filename + "\"");			
		}
	}
	
	@Override
	public Iterator<Element> iterator() {
		return new SAXIterator(data);
	}
	
	public SAXDocument() {
		this.data = null;
	}
	
	public SAXDocument(byte[] data) {
		this.data = data;
	}

}
