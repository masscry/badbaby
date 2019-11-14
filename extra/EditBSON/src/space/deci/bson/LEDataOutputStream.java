package space.deci.bson;

import java.io.DataOutput;
import java.io.FilterOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public
class LEDataOutputStream extends FilterOutputStream implements DataOutput {

	protected int written;

    public LEDataOutputStream(OutputStream out) {
        super(out);
    }

    private void incCount(int value) {
        int temp = written + value;
        if (temp < 0) {
            temp = Integer.MAX_VALUE;
        }
        written = temp;
    }

    public synchronized void write(int b) throws IOException {
        out.write(b);
        incCount(1);
    }

    public synchronized void write(byte b[], int off, int len)
        throws IOException
    {
        out.write(b, off, len);
        incCount(len);
    }

    public void flush() throws IOException {
        out.flush();
    }

    public final void writeBoolean(boolean v) throws IOException {
        out.write(v ? 1 : 0);
        incCount(1);
    }

    public final void writeByte(int v) throws IOException {
        out.write(v);
        incCount(1);
    }

    public final void writeShort(int v) throws IOException {
        out.write((v >>> 0) & 0xFF);
        out.write((v >>> 8) & 0xFF);
        incCount(2);
    }

    public final void writeChar(int v) throws IOException {
        out.write((v >>> 0) & 0xFF);
        out.write((v >>> 8) & 0xFF);
        incCount(2);
    }

    public final void writeInt(int v) throws IOException {
        out.write((v >>>  0) & 0xFF);
        out.write((v >>>  8) & 0xFF);
        out.write((v >>> 16) & 0xFF);
        out.write((v >>> 24) & 0xFF);
        incCount(4);
    }

    private byte writeBuffer[] = new byte[8];

    public final void writeLong(long v) throws IOException {
        writeBuffer[0] = (byte)(v >>>  0);
        writeBuffer[1] = (byte)(v >>>  8);
        writeBuffer[2] = (byte)(v >>> 16);
        writeBuffer[3] = (byte)(v >>> 24);
        writeBuffer[4] = (byte)(v >>> 32);
        writeBuffer[5] = (byte)(v >>> 40);
        writeBuffer[6] = (byte)(v >>> 48);
        writeBuffer[7] = (byte)(v >>> 56);
        out.write(writeBuffer, 0, 8);
        incCount(8);
    }

    public final void writeFloat(float v) throws IOException {
        writeInt(Float.floatToIntBits(v));
    }

    public final void writeDouble(double v) throws IOException {
        writeLong(Double.doubleToLongBits(v));
    }

    public final void writeBytes(String s) throws IOException {
        int len = s.length();
        for (int i = 0 ; i < len ; i++) {
            out.write((byte)s.charAt(i));
        }
        incCount(len);
    }

    public final void writeChars(String s) throws IOException {
        int len = s.length();
        for (int i = 0 ; i < len ; i++) {
            int v = s.charAt(i);
            out.write((v >>> 8) & 0xFF);
            out.write((v >>> 0) & 0xFF);
        }
        incCount(len * 2);
    }

    public final void writeUTF(String str) throws IOException {
    	throw new IOException();
    }

    static int writeUTF(String str, DataOutput out) throws IOException {
    	throw new IOException();
    }

    public final int size() {
        return written;
    }
}
