package space.deci.bson;

import java.util.List;

public abstract class DOMElement implements Element {

	String key;
	
	@Override
	public String GetKey() {
		return key;
	}
	
	public void SetKey(String key) {
		this.key = key;
	}
	
	@Override
	public DataType GetType() {
		return DataType.END;
	}

	@Override
	public double GetNumber() {
		throw new RuntimeException("Invalid Element Type");
	}
	
	@Override
	public String GetString() {
		throw new RuntimeException("Invalid Element Type");
	}
	
	public List<DOMElement> GetDocument()
	{
		throw new RuntimeException("Invalid Element Type");		
	}
	
	public DOMElement(String key) {
		this.key = key;
	}
	
	@Override
	public String toString() {
		return this.GetKey();
	}

}
