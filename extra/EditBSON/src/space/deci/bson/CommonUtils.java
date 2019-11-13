package space.deci.bson;

public class CommonUtils {
	
	public static int findZeroByte(byte[] data, int start)
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

    public static int getInt(byte[] input, int offset) {
    	if (offset + 3 >= input.length)
    	{
    		throw new RuntimeException("Integer is out of bound");
    	}
    	
        int ch4 = input[offset+0];
        int ch3 = input[offset+1];
        int ch2 = input[offset+2];
        int ch1 = input[offset+3];
        int result = ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));
        return result;
    }
    
    public static long getLong(byte[] input, int offset) {
    	if (offset + 7 > input.length)
    	{
    		throw new RuntimeException("Long is out of bound");
    	}
    	
        return (((long)input[offset+7] << 56) +
                ((long)(input[offset+6] & 0xFF) << 48) +
                ((long)(input[offset+5] & 0xFF) << 40) +
                ((long)(input[offset+4] & 0xFF) << 32) +
                ((long)(input[offset+3] & 0xFF) << 24) +
                ((input[offset+2] & 0xFF) << 16) +
                ((input[offset+1] & 0xFF) <<  8) +
                ((input[offset+0] & 0xFF) <<  0));
    }

    public static float getFloat(byte[] input, int offset) {
        return Float.intBitsToFloat(getInt(input, offset));
    }

    public static double getDouble(byte[] input, int offset) {
        return Double.longBitsToDouble(getLong(input, offset));
    }

}
