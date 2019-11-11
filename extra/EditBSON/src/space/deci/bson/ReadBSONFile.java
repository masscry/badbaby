package space.deci.bson;

import java.util.Iterator;

public class ReadBSONFile {

	public static void main(String[] args) {
		
		BSONDocument doc = new BSONDocument();
		
		for (String filename: args) {
			doc.Load(filename);
			
			Iterator<Element> it = doc.iterator();
			
			Element el = it.next();
			
			System.out.println(el.GetKey());	
		}

	}

}
