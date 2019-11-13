package space.deci.bson.test;

import java.util.Iterator;
import space.deci.bson.BSONDocument;
import space.deci.bson.Document;
import space.deci.bson.Element;

public class ReadBSONFile {
	
	private static void PrintElements(Document doc, int offset)
	{
		Iterator<Element> it = doc.iterator();
				
		while (it.hasNext())
		{
			for (int i = 0; i < offset; ++i)
			{
				System.out.print(" ");
			}
			
			Element el = it.next();
			switch(el.GetType())
			{
			case END:
				throw new RuntimeException("Unexpected END!");
			case NUMBER:
				System.out.println(el.GetKey() + ": " + Double.toString(el.GetNumber()));				
				break;
			case STRING:
				System.out.println(el.GetKey() + ": " + el.GetString());				
				break;
			case DOCUMENT:
				System.out.println(el.GetKey() + ": ");
				PrintElements(el.GetDocument(), offset + 2);
				break;
			default:
				throw new RuntimeException("Unsupported element type");
			}
		}
	}

	public static void main(String[] args) {
		
		BSONDocument doc = new BSONDocument();
		
		for (String filename: args) {
			doc.Load(filename);			
			PrintElements(doc, 0);
		}

	}

}
