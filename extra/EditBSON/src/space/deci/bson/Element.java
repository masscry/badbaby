package space.deci.bson;

public interface Element {

	DataType GetType();
	String GetKey();	

	double GetNumber();	
	String GetString();
	Document GetDocument();
	
}
