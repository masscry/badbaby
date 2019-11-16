package space.deci.bson;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
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

	public DOMDocument(String key, SAXDocument parser)
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
	
	public byte[] ToByteArrayAsRoot()
	{
		ByteArrayOutputStream resultBuild = new ByteArrayOutputStream();	
		
		Iterator<DOMElement> it = elements.iterator();
		while(it.hasNext())
		{
			DOMElement elem = it.next();
			try
			{
				resultBuild.write(elem.ToByteArray());
			}
			catch (IOException ioexcept)
			{
				throw new RuntimeException("Failed to write to ByteArray stream");
			}
		}			
		
		ByteBuffer tmp = ByteBuffer.allocate(resultBuild.size() + 1);
		
		tmp.order(ByteOrder.LITTLE_ENDIAN);
		tmp.put(resultBuild.toByteArray());
		tmp.put(DataType.END.ID());
		return tmp.array();
	}
	
	@Override
	public byte[] ToByteArray()
	{
		byte[] byteKey = this.GetKey().getBytes(StandardCharsets.UTF_8);		
		byte[] byteValue = this.ToByteArrayAsRoot();
		
		ByteBuffer tmp = ByteBuffer.allocate(1 + byteKey.length + 1 + 4 + byteValue.length);
		
		tmp.order(ByteOrder.LITTLE_ENDIAN);
		tmp.put(DataType.DOCUMENT.ID());
		tmp.put(byteKey);
		tmp.put(DataType.END.ID());
		tmp.putInt(byteValue.length + 4);
		tmp.put(byteValue);
		return tmp.array();
	}
	
}
