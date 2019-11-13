package space.deci.bson;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

public class BSONElement implements Element {

	private final byte[] data;
	private final int offset;	

	private String key;
	private int keyEnd;

	public BSONElement(byte[] data, int offset) {
		this.data = data;
		this.offset = offset;
		this.key = null;
		this.keyEnd = -1;
	}
	
	private void updateKeyEnd()
	{
		if (this.keyEnd < 0)
		{
			this.keyEnd = CommonUtils.findZeroByte(this.data, this.offset + 1);
		}		
	}
	
	@Override
	public String GetKey() {
		if (this.key == null)
		{
			this.updateKeyEnd();					
			this.key = new String(
				this.data,
				this.offset + 1,
				this.keyEnd - (this.offset + 1),
				StandardCharsets.UTF_8
			);
		}
		return this.key;
	}
	
	@Override
	public double GetNumber() {
		this.updateKeyEnd();		
		return CommonUtils.getDouble(this.data, this.keyEnd + 1);
	}
	
	@Override
	public String GetString() {		
		this.updateKeyEnd();
		int size = CommonUtils.getInt(this.data, this.keyEnd+1);
		
		String result = new String(
			this.data,
			this.keyEnd + 5,
			size-1,
			StandardCharsets.UTF_8
		);
		return result;
	}

	@Override
	public Document GetDocument() {
		this.updateKeyEnd();
		int size = CommonUtils.getInt(this.data, this.keyEnd + 1);		
		return new BSONDocument(Arrays.copyOfRange(this.data, this.keyEnd+5, this.keyEnd+size+1));
	}
	
	@Override
	public DataType GetType() {
		return DataType.fromID(this.data[this.offset]);		
	}

}
