package DStruct;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Timer;

import javax.script.ScriptContext;
import javax.script.ScriptEngine;
import javax.script.ScriptEngineManager;
import javax.script.ScriptException;

import org.apache.commons.lang3.tuple.Pair;
import org.python.core.Py;
import org.python.core.PyFunction;
import org.python.core.PyInteger;
import org.python.core.PyObject;
import org.python.core.PyString;
import org.python.util.PythonInterpreter;

import com.caucho.quercus.script.QuercusScriptEngineFactory;

import Utils.Config;
import Utils.DB;

public class Extractor {
	
	public static HashMap<String, Extractor> extractors = new HashMap<String, Extractor>();
	
	public static PythonInterpreter interpreter = new PythonInterpreter();
	
	String extname;
	
	String type;
	
	public ArrayList<Pair<String, String>> config;
	
	public HashMap<String, String> configmap = new HashMap<String, String>();
	
	public static Factor factors; 
	
	
	public static QuercusScriptEngineFactory phpfactory; 
	
	public static ScriptEngine phpengine;
		
	public static void init() throws SQLException{
		
		factors = new Factor();
		
		interpreter.execfile("lib/jeo.py");
	    PyFunction pyFuntion = (PyFunction) interpreter.get("_conndb", PyFunction.class);
	    pyFuntion.__call__(new PyString(DB.connstr()));
	    
	    pyFuntion = (PyFunction) interpreter.get("_init_path", PyFunction.class);
	    //pyFuntion.__call__(new PyString("."));
	    pyFuntion.__call__(new PyString(System.getProperty("user.dir") + "/" + "lib"));
	    pyFuntion.__call__(new PyString(System.getProperty("user.dir") + "/" + Config.TASK_DIR));
	    
	    pyFuntion = (PyFunction) interpreter.get("_init_factors", PyFunction.class);
	    pyFuntion.__call__(Py.java2py(factors));
	    
	}
	
	public Extractor(String _extname, ArrayList<Pair<String, String>> _config){
		extname = _extname;
		config = _config;
		
		for(Pair<String, String> setting: config){
			configmap.put(setting.getLeft(), setting.getRight());
		}
		
		type = configmap.get("TYPE");
		Config.info("Creating extractor [TYPE=" + type + "] " + extname);
		
	}
	
	public void execute_SQL() throws SQLException{
		Config.debug("    SQL=" + configmap.get("EXEC"));
		int count = DB.executeUpdate(configmap.get("EXEC"));
		Config.info("       #UPDATE=" + count);
	}
	
	public void execute_PYTHON() throws SQLException, IOException{
		
	    PyFunction pyFuntion = null;
	    
	    if(type.equals("PYTHON")){
	    	pyFuntion = (PyFunction) interpreter.get("_callfunc", PyFunction.class);
	    }else if(type.equals("PHP")){
	    	pyFuntion = (PyFunction) interpreter.get("_callphp", PyFunction.class);
	    }
	    
	    
	    ArrayList<Relation> outrels = new ArrayList<Relation>();
	    if(configmap.containsKey("IMPACTREL")){
	    	String output = configmap.get("IMPACTREL");
	    	String[] outputs = output.split(" ");
	    	for(String rname : outputs){
	    		if(!rname.equals("") && !Relation.relations.containsKey(rname)){
	    			Config.error("Extractor" + extname + "'s output relation " + rname + " does not exist...");
	    			Runtime.getRuntime().exit(-1);
	    		}
	    		if(!rname.equals("")){
	    			Relation.relations.get(rname).clearStats();
	    			outrels.add(Relation.relations.get(rname));
	    		}
	    	}
	    }
	    factors.clearStats();
	    
	    if(type.equals("PYTHON")){
	    	pyFuntion.__call__(new PyString(configmap.get("INPUT")), 
	    				   new PyString(configmap.get("FUNC")),
	    				   Py.java2py(outrels));
	    }else{
	    	pyFuntion.__call__(new PyString(configmap.get("INPUT")), 
 				   new PyString(Config.getAbsPath(configmap.get("FUNC"))),
 				   Py.java2py(outrels), Py.java2py(factors));
	    	
	    }
	    
	    for(Relation r : outrels){
	    	r.reportStats();
	    }
	    Config.info("Start batch-inserting factors.");
	    factors.batchExec();
	    factors.reportStats();

	}
	
	public void execute() throws SQLException, IOException{
		
		long startTime = System.currentTimeMillis();

		Config.info("Executing extractor [TYPE=" + type + "] " + extname);
		
		if(type.equals("SQL")){
			execute_SQL();
		}
		
		if(type.equals("PYTHON") || type.equals("PHP")){
			execute_PYTHON();
		}
		
		DB.conn.commit();
		
		Config.info("Finish extractor [TYPE=" + type + "] " + extname + "   TIME=" 
					+ 1.0*((new Date()).getTime() - startTime)/1000 + " seconds\n");
		
	}
	
}




