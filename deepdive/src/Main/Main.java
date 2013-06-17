package Main;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.logging.Logger;

import javax.script.ScriptContext;
import javax.script.ScriptEngine;
import javax.script.ScriptEngineManager;
import javax.script.ScriptException;

import org.apache.commons.lang.exception.NestableException;
import org.apache.commons.lang3.tuple.Pair;
import org.apache.commons.logging.LogFactory;
import org.apache.commons.logging.impl.LogFactoryImpl;
import org.python.core.Py;
import org.python.core.PyFunction;
import org.python.core.PyString;

import DStruct.Extractor;
import DStruct.Relation;
import Utils.Config;
import Utils.DB;
import Utils.INIParser;

import com.caucho.quercus.script.QuercusScriptEngineFactory;


public class Main {

	/**
	 * @param args
	 * @throws NestableException 
	 * @throws SQLException 
	 * @throws IOException 
	 */
	public static void main(String[] args) throws NestableException, SQLException, IOException {
		
		BridgeToElly Elly = new BridgeToElly();
		Elly.elly.init_java();

		if(args.length != 2){
			System.out.println("Usage java -jar jeo.jar <TASK_RELATIVE_FOLDER> <PIPELINE_TO_RUN>");
			Runtime.getRuntime().exit(-1);
		}
		
		String TORUN = args[1];
		
		Config.TASK_DIR = args[0];
		
		Config.info("Task folder = " + Config.TASK_DIR);
		Config.info("Pipeline to run = " + TORUN);
		
		DB.init();
		
		Extractor.init();
		
		String INI_DATAMODEL = Config.TASK_DIR + "/datamodel.cfg";
		INIParser dm_parser = new INIParser(INI_DATAMODEL);
		HashMap<String, ArrayList<Pair<String, String>>> datamodel = dm_parser.parseINI();
		
		for(String relname: datamodel.keySet()){
			Relation.relations.put(relname, new Relation(relname, datamodel.get(relname)));
		}
		
		String INI_EXTRACTOR = Config.TASK_DIR + "/extractor.cfg";
		INIParser ext_parser = new INIParser(INI_EXTRACTOR);
		HashMap<String, ArrayList<Pair<String, String>>> extmodel = ext_parser.parseINI();
		
		for(String extname: extmodel.keySet()){
			Extractor.extractors.put(extname, new Extractor(extname, extmodel.get(extname)));
		}
		Config.info("Finished Creating Extractors!\n");
		
		String INI_PIPELINE = Config.TASK_DIR + "/pipeline.cfg";
		INIParser ext_pipeline = new INIParser(INI_PIPELINE);
		HashMap<String, ArrayList<Pair<String, String>>> pipelines = ext_pipeline.parseINI();
		
		if(!pipelines.containsKey(TORUN)){
			Config.error("Pipeline " + TORUN + " does not exist...");
			Runtime.getRuntime().exit(-1);
		}
		
		for(Pair<String, String> taskp : pipelines.get(TORUN)){	
			String task = taskp.getLeft();
			if(task.equals("ELEMENTARY|RUN")){
				continue;
			}
			if(!Extractor.extractors.containsKey(task)){
				Config.error("Extractor " + task + " does not exist...");
				Runtime.getRuntime().exit(-1);
			}			
		}
		
		for(Pair<String, String> taskp : pipelines.get(TORUN)){	
			String task = taskp.getLeft();
			if(task.equals("ELEMENTARY|RUN")){
				Elly.addFactors();
				Elly.addVariables();
				
				for(int i=0;i<200;i++){
					Elly.elly.runInference();
				}
				
				Elly.dumpModel();
				Elly.dumpRS();
				
				DB.conn.commit();
			}else{
				Extractor.extractors.get(task).execute();
			}
		}
		
		Config.info("Shutting down HSQLDB;");
		
		for(String relname: datamodel.keySet()){
			DB.execute("DROP INDEX IDINDEX_" + relname + " IF EXISTS;");
    	}
		
		
		DB.conn.close();
		
		PyFunction pyFuntion = (PyFunction) Extractor.interpreter.get("_close", PyFunction.class);
    	pyFuntion.__call__();
    	//DB.execute("SHUTDOWN;");
		//DB.conn.close();
    	
	}

}



