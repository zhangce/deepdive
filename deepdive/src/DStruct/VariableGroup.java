package DStruct;

public class VariableGroup {

	public String column;
	public Object value;
	public Relation relation;
	
	public VariableGroup(String _column, Object _value, Relation _relation){
		column = _column;
		value = _value;
		relation = _relation;
	}
	
}
