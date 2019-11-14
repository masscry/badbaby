package space.deci.bson;

public interface Element {

	String GetKey();
	
	DataType GetType();
	double GetNumber();	
	String GetString();
		
}
