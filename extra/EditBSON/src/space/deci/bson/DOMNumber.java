package space.deci.bson;

import java.nio.charset.StandardCharsets;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

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
	
	@Override
	public byte[] ToByteArray()
	{
		byte[] byteKey = this.GetKey().getBytes(StandardCharsets.UTF_8);
		ByteBuffer tmp = ByteBuffer.allocate(1 + byteKey.length + 1 + 8);
		
		tmp.order(ByteOrder.LITTLE_ENDIAN);
		tmp.put(DataType.NUMBER.ID()); // number
		tmp.put(byteKey);    // key
		tmp.put(DataType.END.ID());
		tmp.putDouble(this.GetNumber());		
		return tmp.array();
	}
	
}
