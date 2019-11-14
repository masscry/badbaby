package space.deci.bson;

public interface Document extends Iterable<Element> {
	void Load(String filename);
}
