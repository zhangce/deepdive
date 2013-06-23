package Utils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Set;

import org.apache.commons.configuration.HierarchicalINIConfiguration;
import org.apache.commons.configuration.SubnodeConfiguration;
import org.apache.commons.lang.exception.NestableException;
import org.apache.commons.lang3.tuple.Pair;

public class INIParser {

	String filename;
	
	public INIParser(String _filename){
		filename = _filename;
	}
	
	public HashMap<String, ArrayList<Pair<String, String>>> parseINI() throws NestableException{
		
		HashMap<String, ArrayList<Pair<String, String>>> rs = new HashMap<String, ArrayList<Pair<String, String>>>();
		
		
		HierarchicalINIConfiguration.setDefaultListDelimiter('`');
		
		HierarchicalINIConfiguration iniConfObj = new HierarchicalINIConfiguration(filename);
		
		Set<String> setOfSections = iniConfObj.getSections();
		Iterator<String> sectionNames = setOfSections.iterator();

		while(sectionNames.hasNext()){

			String sectionName = sectionNames.next().toString();
		 
		 	rs.put(sectionName, new ArrayList<Pair<String, String>>());

		 	SubnodeConfiguration sObj = iniConfObj.getSection(sectionName);
		 	Iterator<String> it1 = sObj.getKeys();
		 	
		 	while (it1.hasNext()) {
			 	String key = it1.next().toString();
			 	rs.get(sectionName).add(Pair.of(key, sObj.getString(key.toString())));
		 	}
		
		}
		
		return rs;
	}
}
