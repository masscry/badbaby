package space.deci.bson;

public class DOMNumber extends DOMElement {
	
	private double value;

	@Override
	public DataType GetType() {
		return DataType.NUMBER;
	}
	
	@Override
	public double GetNumber() {
		return this.value;
	}
	
	public void SetValue(double value) {
		this.value = value;
	}
	
	public DOMNumber(Element element) {
		super("");
		this.value = element.GetNumber();
	}
	
	public DOMNumber(String key, Element element) {
		super(key);
		this.value = element.GetNumber();
	}
	
}
