package space.deci.bson;

public interface Element {

	DataType GetType();
	String GetKey();	
	Double GetNumber();	
	String GetString();
	Document GetDocument();
	
}
