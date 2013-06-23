package python.DStruct;

import java.sql.Array;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.logging.Logger;

import javax.script.ScriptEngine;

import org.apache.commons.lang3.tuple.Pair;
import org.apache.commons.logging.Log;

import com.caucho.quercus.Quercus;
import com.caucho.quercus.env.ConstStringValue;
import com.caucho.quercus.script.QuercusScriptEngine;
import com.caucho.quercus.script.QuercusScriptEngineFactory;
import com.google.common.base.Joiner;

import python.Utils.Config;
import python.Utils.DB;

public class Relation {
	
	public static HashMap<String, Relation> relations = new HashMap<String, Relation>();
	
	public String relname;
	
	public ArrayList<Pair<String, String>> config;
	
	public HashMap<String, String> configmap = new HashMap<String, String>();
	
	public HashMap<String, String> column2decl = new HashMap<String, String>();
	
	public ArrayList<String> columns = new ArrayList<String>();
	
	public ArrayList<String> domain = new ArrayList<String>();
	
	public HashMap<String,Integer> domainvalue2index = new HashMap<String,Integer>();
	
	PreparedStatement insertps;
	
	HashMap<String, PreparedStatement> col2setevidps = new HashMap<String, PreparedStatement>();
	
	HashMap<String, PreparedStatement> col2getvidsps = new HashMap<String, PreparedStatement>();
	
	int n_update_setevid, n_update_insert_tuple;
	
	public void reportStats(){
		Config.info("       Relation " + relname + ": #insert_tuple=" + n_update_insert_tuple + 
				"   #set_evid_tuple=" + n_update_setevid);
	}
	
	public void clearStats(){
		n_update_setevid = 0;
		n_update_insert_tuple = 0;
	}
	
	public String getSQLInsertPS(){
		ArrayList<String> qs = new ArrayList<String>();
		ArrayList<String> cols = new ArrayList<String>();
		for(String col: columns){
			if(col.equals("id")){
				continue;
			}
			cols.add(col);
			qs.add("?");
		}
		String sql = "INSERT INTO " + relname + "(" + Joiner.on(",").join(cols) + ")" 
				+ " VALUES " + "(" + Joiner.on(",").join(qs) + ");";
		return sql;
	}
	
	public String getSQLSetVarPS(String colname){
		String sql = "UPDATE " + relname + " SET varvalue_ = ?, isevid_ = True, prob_ = ?  WHERE " + colname + " = ?";
		return sql;
	}
	
	public String getSQLGetVIDPS(String colname){
		String sql = "SELECT id FROM " + relname + " WHERE " + colname + " = ?";
		return sql;
	}
	
	public String getSQLCreateTable(){
		
		ArrayList<String> cols = new ArrayList<String>();
		for(String col: columns){
			cols.add(col + " " + column2decl.get(col));
		}
		
		if(configmap.containsKey("_var")){
			cols.add("varvalue_ TEXT DEFAULT NULL");
			cols.add("prob_ FLOAT DEFAULT 0");
			cols.add("isevid_  BOOLEAN DEFAULT FALSE");
		}else{
			cols.add("varvalue_ TEXT DEFAULT '1'");
			cols.add("prob_ FLOAT DEFAULT 1");
			cols.add("isevid_  BOOLEAN DEFAULT TRUE"); 
				// if there are no variables associated with a relation, then we assume
			    // that they are all TRUE evidence
		}
		
		String sql = "CREATE TABLE " + relname + "(" + Joiner.on(",").join(cols) + ");";
		
		return sql;
		
	}
	
	public String getSQLCreateTextTable(String filename){
		
		ArrayList<String> cols = new ArrayList<String>();
		for(String col: columns){
			if(col.equals("id")){
				continue;
			}
			cols.add(col + " " + column2decl.get(col));
		}
		
		String sql = "CREATE TEXT TABLE " + relname+"_evid" + "(" + Joiner.on(",").join(cols) + ");";
		
		return sql;
		
	}
	
	public String getSQLSetTextTablePath(String filename){
		
		return "SET TABLE " + relname + "_evid" + " SOURCE \"" + filename + ";fs=\\t;encoding=UTF-8\"";
		
	}
	
	public VariableGroup vars(String column, Object bindvalue){
		return new VariableGroup(column, bindvalue, this);
	}
	
	public String getSQLSetCopyTablePath(){
		
		ArrayList<String> cols = new ArrayList<String>();
		for(String col: columns){
			if(col.equals("id")){
				continue;
			}
			cols.add(col);
		}
		
		return "INSERT INTO " + relname + "(" + Joiner.on(",").join(cols) + ")" + " SELECT * FROM " + relname + "_evid;";
		
	}
	
	/*
	public void insertTuple(ConstStringValue... cols) throws SQLException{
		
		for(int i=0;i<cols.length;i++){
			insertps.setObject(i+1, cols[i].getValue());
		}
		
		this.n_update_insert_tuple += insertps.executeUpdate();
	}
	*/
	
	public void insertTuple(Object... cols) throws SQLException{
		
		for(int i=0;i<cols.length;i++){
			insertps.setObject(i+1, cols[i]);
		}
		
		this.n_update_insert_tuple += insertps.executeUpdate();
	}
	
	/*
	public void setEvid(ConstStringValue column, ConstStringValue bindvalue, ConstStringValue varvalue) throws SQLException{
		
		//System.out.println(column.getValue() + "    ~~~~    " + bindvalue.getValue());
		
		this.col2setevidps.get(column.getValue()).setObject(1, varvalue.getValue());
		this.col2setevidps.get(column.getValue()).setObject(2, bindvalue.getValue());
		
		this.n_update_setevid += this.col2setevidps.get(column.getValue()).executeUpdate();
	}*/
	
	public void setEvid(String column, Object bindvalue, String varvalue) throws SQLException{
		
		this.col2setevidps.get(column).setObject(1, varvalue);
		this.col2setevidps.get(column).setObject(2, 1.0);
		this.col2setevidps.get(column).setObject(3, bindvalue);
		
		this.n_update_setevid += this.col2setevidps.get(column).executeUpdate();
	}
	
	public void setEvid(String column, Object bindvalue, String varvalue, Double prob) throws SQLException{
		
		this.col2setevidps.get(column).setObject(1, varvalue);
		this.col2setevidps.get(column).setObject(2, prob);
		this.col2setevidps.get(column).setObject(3, bindvalue);
		
		this.n_update_setevid += this.col2setevidps.get(column).executeUpdate();
	}
	
	public ArrayList<Integer> getVIDs(String column, Object bindvalue) throws SQLException{
		
		ArrayList<Integer> vids = new ArrayList<Integer>();
		this.col2getvidsps.get(column).setObject(1, bindvalue);
		
		ResultSet rs = this.col2getvidsps.get(column).executeQuery();
		
		while(rs.next()){
			vids.add(rs.getInt(1));
		}
		
		return vids;
		
	}
	
	public Relation(String _relname, ArrayList<Pair<String, String>> _config) throws SQLException{
		relname = _relname;
		config = _config;
		
		config.add(Pair.of("id", "INT GENERATED BY DEFAULT AS SEQUENCE VARSEQ"));
		//config.add(Pair<String, String>.("id", "INT GENERATED BY DEFAULT AS SEQUENCE VARSEQ PRIMARY KEY"));
		
		for(Pair<String, String> setting: config){
			
			String col = setting.getLeft();
			
			if(col.startsWith("_")){	// if control property
				configmap.put(col, setting.getRight());
				continue;
			}
			
			columns.add(col);
			column2decl.put(col, setting.getRight());
		}
		
		Config.info("Creating relation " + relname);
		
		DB.execute("DROP TABLE IF EXISTS " + relname);
		
		Config.debug("    SQL=" + getSQLCreateTable());
		
		DB.execute(getSQLCreateTable());
		
		Config.info("Register Insert Prepared Statement for " + relname);
		
		Config.debug("    PS=" + getSQLInsertPS());
		
		this.insertps = DB.conn.prepareStatement(getSQLInsertPS());
		
		Config.info("Register Set Evid Prepared Statement for " + relname);
		
		for(String col : columns){
			Config.debug("    PS=" + getSQLSetVarPS(col));
			col2setevidps.put(col, DB.conn.prepareStatement(getSQLSetVarPS(col)));
		
			Config.debug("    PS=" + this.getSQLGetVIDPS(col));
			col2getvidsps.put(col,  DB.conn.prepareStatement(this.getSQLGetVIDPS(col)));
		}
		
		for(Pair<String, String> setting: config){
			
			String col = setting.getLeft();
			
			if(!col.equals("_data")){
				continue;
			}
			
			String filename = Config.getAbsPath(setting.getRight());
			
			Config.info("Loading TSV into " + relname + " from " + filename);
			DB.execute("DROP TABLE IF EXISTS " + relname + "_evid");
			
			Config.debug("    SQL=" + this.getSQLCreateTextTable(filename));
			Config.debug("    SQL=" + this.getSQLSetTextTablePath(filename));
			Config.debug("    SQL=" + this.getSQLSetCopyTablePath());
			
			DB.execute(this.getSQLCreateTextTable(filename));
			DB.execute(this.getSQLSetTextTablePath(filename));
			int count = DB.executeUpdate(this.getSQLSetCopyTablePath());
			
			Config.info("       #UPDATE=" + count);
		}
		
		
		Config.info("Creating Index for " + relname + "(id)");
		
		//DB.execute("DROP INDEX IDINDEX_" + relname + " IF EXISTS;");
		DB.execute("DROP INDEX IDINDEX_" + relname + " IF EXISTS");
		
		Config.debug("    SQL = " + "CREATE INDEX IDINDEX_" + relname + " ON " + relname + "(id);");
		DB.execute("CREATE INDEX IDINDEX_" + relname + " ON " + relname + "(id);");
		
		DB.conn.commit();
		
		int ndomain = -1;
		if(configmap.containsKey("_var")){
			Config.info("Parsing variable domain for relation " + relname);
			String[] values = configmap.get("_var").split(" ");
			for(String value : values){
				if(value.length() > 0){
					domain.add(value);
					ndomain = ndomain + 1;
					domainvalue2index.put(value, ndomain);
				}
			}
			Config.info("       #VALUE=" + domain.size());
		}
		
		Config.info("Finished Init Relation " + relname + "\n");
		
	}
	
}
