package space.deci.bson;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;

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
	
	@Override
	public byte[] ToByteArray()
	{
		byte[] byteKey = this.GetKey().getBytes(StandardCharsets.UTF_8);
		byte[] byteValue = this.GetString().getBytes(StandardCharsets.UTF_8);
				
		ByteBuffer tmp = ByteBuffer.allocate(
				1 + // 0x2
				byteKey.length + 1 + // zero terminated string
				4 + byteValue.length + 1 // length + zero terminated string
		);
		
		tmp.order(ByteOrder.LITTLE_ENDIAN);
		tmp.put(DataType.STRING.ID()); // string
		tmp.put(byteKey);    // key
		tmp.put(DataType.END.ID());
		tmp.putInt(byteValue.length + 1);
		tmp.put(byteValue);
		tmp.put(DataType.END.ID());
				
		return tmp.array();
	}

}
