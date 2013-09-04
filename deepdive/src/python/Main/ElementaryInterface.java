package python.Main;

import java.util.HashMap;

public class ElementaryInterface {

	public HashMap<Integer, Double> model = new HashMap<Integer, Double>();
	
	public HashMap<Integer, Boolean> model_mask = new HashMap<Integer, Boolean>();
	
	public Double getModel(Integer i){
		return model.get(i);
	}
	
	public void setModel(Integer i, Double d){
		model.put(i, d);
	}
	
	public Boolean getModelMask(Integer i){
		return model_mask.get(i);
	}
	
	public void setModelMask(Integer i, Boolean d){
		model_mask.put(i, d);
	}
	
	public void addFactor(long fid, int pid, long nvar, long aux){
		
	}

	

}
