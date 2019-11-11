package space.deci.bson;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Iterator;

public class BSONElement implements Element, Iterator<Element> {
	
	private final byte[] data;
	private int offset;
	
	private int findZeroByte(int start)
	{
		for (int i = start; i < data.length; ++i)
		{
			if (data[i] == 0x00)
			{
				return i;
			}
		}
		throw new RuntimeException("Zero not found!");
	}
	
	@Override
	public Document GetDocument() {
		// TODO Auto-generated method stub
		return null;
	}
	
	@Override
	public String GetKey() {
		int keyStart = this.offset + 1;
		int keyEnd = this.findZeroByte(keyStart);
		
		byte[] key = Arrays.copyOfRange(data, keyStart, keyEnd);
		
		return new String(key, StandardCharsets.UTF_8);
	}
	
	@Override
	public Double GetNumber() {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public String GetString() {
		// TODO Auto-generated method stub
		return null;
	}
	
	@Override
	public DataType GetType() {
		return DataType.fromID(this.data[this.offset]);		
	}
	
	@Override
	public boolean hasNext() {
		return this.data[this.offset] != DataType.END.ID();
	}
	
	@Override
	public Element next() {
		if (this.offset == -1)
		{
			return new BSONElement(data, 0);
		}
		return null;
	}
	
	private BSONElement(byte[] data, int offset)
	{
		this.data = data;
		this.offset = offset;
	}
	
	public BSONElement(byte[] data) {
		this.data = data;
		this.offset = -1;
	}
	
}
