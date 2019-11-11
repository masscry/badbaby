package space.deci.bson;

public enum DataType {
	
	END(0x00),
	NUMBER(0x01),
	STRING(0x02),
	DOCUMENT(0x03);
	
	private final byte id;
	
	DataType(int id)
	{
		this.id = (byte) id;
	}
	
	public byte ID()
	{
		return this.id;
	}
	
	public static DataType fromID(byte id)
	{
		switch (id)
		{
		case 0:
			return END;
		case 1:
			return NUMBER;
		case 2:
			return STRING;
		case 3:
			return DOCUMENT;
		default:
			throw new RuntimeException("Unknown DataType: " + id);
		}
	}

}
