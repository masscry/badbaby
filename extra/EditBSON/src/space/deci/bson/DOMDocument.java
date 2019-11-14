package space.deci.bson;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class DOMDocument extends DOMElement {
	
	List<DOMElement> elements;
	
	@Override
	public DataType GetType() {
		return DataType.DOCUMENT;
	}

	public List<DOMElement> GetDocument()
	{
		return this.elements;
	}

	public DOMDocument(SAXElement element)
	{
		this("", element);
	}
	
	public DOMDocument(String key, SAXElement element)
	{
		this(key, element.GetDocument());
	}

	public DOMDocument(String key, SAXParser parser)
	{
		super(key);
		
		this.elements = new ArrayList<DOMElement>();
		
		Iterator<SAXElement> it = parser.iterator();
		
		while(it.hasNext())
		{
			SAXElement item = it.next();
			switch (item.GetType())
			{
			case NUMBER:
				this.elements.add(new DOMNumber(item.GetKey(), item));
				break;
			case STRING:
				this.elements.add(new DOMString(item.GetKey(), item));
				break;
			case DOCUMENT:
				this.elements.add(new DOMDocument(item.GetKey(), item));
				break;
			default:
				throw new RuntimeException("Unknown Element Type");
			}
		}
	}
	
}
