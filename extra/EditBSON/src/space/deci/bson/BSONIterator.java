package space.deci.bson;

import java.util.Iterator;
import java.util.NoSuchElementException;

public class BSONIterator implements Iterator<Element> {
	
	private final byte[] data;
	private int offset;
			
	@Override
	public boolean hasNext() {
		return this.data[this.offset] != DataType.END.ID();
	}
	
	@Override
	public Element next() {		
		int currentOffset = this.offset;
		switch (DataType.fromID(this.data[this.offset]))
		{
		case END:
			throw new NoSuchElementException("Last element in document reached!");
		case NUMBER:
			this.offset = CommonUtils.findZeroByte(this.data, this.offset+1) + 1;
			this.offset += 8;
			break;
		case STRING:
			this.offset = CommonUtils.findZeroByte(this.data, this.offset+1) + 1;
			this.offset += 4 + CommonUtils.getInt(this.data, this.offset);
			break;
		case DOCUMENT:
			this.offset = CommonUtils.findZeroByte(this.data, this.offset+1) + 1;
			this.offset += CommonUtils.getInt(this.data, this.offset);			
			break;
		default:
			throw new RuntimeException("Can't deal with this type...");
		}
		return new BSONElement(data, currentOffset);
	}
	
	public BSONIterator(byte[] data) {
		this.data = data;
		this.offset = 0;
	}
	
}
