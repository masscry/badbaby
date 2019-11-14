package space.deci.bson;

public class DOMString extends DOMElement {
	
	private String value;
	
	@Override
	public DataType GetType() {
		return DataType.STRING;
	}
	
	@Override
	public String GetString() {
		return this.value;
	}
	
	public void SetString(String value) {
		this.value = value;
	}
	
	public DOMString(Element element) {
		super("");
		this.value = element.GetString();
	}
	
	public DOMString(String key, Element element) {
		super(key);
		this.value = element.GetString();
	}

}
