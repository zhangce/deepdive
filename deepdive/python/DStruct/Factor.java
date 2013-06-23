package DStruct;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;

import javax.sql.rowset.serial.SerialBlob;

import org.apache.commons.lang3.SerializationUtils;

import com.google.common.base.Joiner;

import Utils.Config;
import Utils.DB;

public class Factor {
	
	public int factorid;
	
	public int nfactor, nedge;
	
	
	public void awesome(){
		System.out.println("~~~~~");
	}
	
	public PreparedStatement template_v2f_insertps;
	public PreparedStatement template_factors_insertps;
	
	public PreparedStatement densetemplate_v2f_insertps;
	public PreparedStatement densetemplate_factors_insertps;
	
	public static String densetable_template_f2v_ = "densetemplate_f2v_";
	public static String densetable_template_factors = "densetemplate_factors_";
	
	public static String table_template_f2v_ = "template_f2v_";
	public static String table_template_factors = "template_factors_";
	
	public static BufferedWriter file_template_f2v_;
	public static BufferedWriter file_template_factors_;
	
	public static String filename_template_f2v_;
	public static String filename_template_factors_;


	public void reportStats(){
		Config.info("       New Factors " + ": #n_factor=" + nfactor + 
				"   #n_edge=" + nedge);
	}
	
	public void clearStats() throws IOException{
		nfactor = 0;
		nedge = 0;
		
		filename_template_f2v_ = Config.getNextTmpFile();
		filename_template_factors_ = Config.getNextTmpFile();
		
		//file_template_f2v_ = new BufferedWriter(new FileWriter(System.getProperty("user.dir") + "/" + filename_template_f2v_));
		//file_template_factors_ = new BufferedWriter(new FileWriter(System.getProperty("user.dir") + "/" + filename_template_factors_));
	}
	
	
	public Factor() throws SQLException{
		template_v2f_insertps = DB.conn.prepareStatement("INSERT INTO template_f2v_ (fid, pos, vid, label) VALUES (?,?,?,?);");
		template_factors_insertps = DB.conn.prepareStatement("INSERT INTO template_factors_ (fid, feature, weight, nvar) VALUES (?,?,?,?);");
		
		densetemplate_v2f_insertps = DB.conn.prepareStatement("INSERT INTO densetemplate_f2v_ (fid, pos, vid) VALUES (?,?,?);");
		densetemplate_factors_insertps = DB.conn.prepareStatement("INSERT INTO densetemplate_factors_ (fid, nvar, auxinfo) VALUES (?,?,?);");
		
		factorid = 0;
		nfactor = 0;
		nedge = 0;
	}
	
	HashMap<String, SerialBlob> auxcache = new HashMap<String, SerialBlob>();
	
	public void addDenseTemplateFactor(ArrayList<ArrayList<Integer>> VARSS,
			ArrayList<ArrayList<String>> VALUESS, ArrayList<String> WEIGHTS) throws SQLException{
		
		//densetemplate_factors_insertps.setInt(1, fid);
		//template_factors_insertps.setDouble(2, feature);
		//template_factors_insertps.setString(3, weight);
		//template_factors_insertps.setInt(4, vars.size());
		
		//nfactor += template_factors_insertps.executeUpdate();
		
		SerialBlob blob = null;
		
		String SIGNATURE = VALUESS.toString() + "|||" + WEIGHTS.toString();
		if(auxcache.containsKey(SIGNATURE)){
			blob = auxcache.get(SIGNATURE);
		}else{
			HashMap<String, byte[]> aux = new HashMap<String, byte[]>();
			//aux.put("values", SerializationUtils.serialize(VALUESS));
			//aux.put("features", SerializationUtils.serialize(FEATURES));
			aux.put("weights", SerializationUtils.serialize(WEIGHTS));
			
			auxcache.put(SIGNATURE, new SerialBlob(SerializationUtils.serialize(aux)));
			blob = auxcache.get(SIGNATURE);
		}
					
		for(ArrayList<Integer> vars : VARSS){
			int fid = (++factorid);
			
			for(int i=0;i<vars.size();i++){
				densetemplate_v2f_insertps.setInt(1, fid);
				densetemplate_v2f_insertps.setInt(2, i);
				densetemplate_v2f_insertps.setInt(3, vars.get(i).intValue());
				
				nedge += densetemplate_v2f_insertps.executeUpdate();
				
			}
			
			densetemplate_factors_insertps.setInt(1, fid);
			densetemplate_factors_insertps.setInt(2, vars.size());
			densetemplate_factors_insertps.setBlob(3, blob);
			
			nfactor += densetemplate_factors_insertps.executeUpdate();
		}
		
		
	}
	
	public void addTemplateFactor(ArrayList<Integer> vars, ArrayList<String> values, Double feature, String weight) throws SQLException, IOException{
		
		int fid = (++factorid);
		
		//template_factors_insertps.setInt(1, fid);
		//template_factors_insertps.setDouble(2, feature);
		//template_factors_insertps.setString(3, weight);
		//template_factors_insertps.setInt(4, vars.size());
		
		//nfactor += template_factors_insertps.executeUpdate();
		
		file_template_factors_.write(fid + "\t" + feature + "\t" + weight + "\t" + vars.size() + "\n");
		
		nfactor ++;
		
		for(int i=0;i<vars.size();i++){
			//template_v2f_insertps.setInt(1, fid);
			//template_v2f_insertps.setInt(2, i);
			//template_v2f_insertps.setInt(3, vars.get(i).intValue());
			//template_v2f_insertps.setString(4, values.get(i));
			//nedge += template_v2f_insertps.executeUpdate();
			
			file_template_f2v_.write(fid + "\t" + i + "\t" + vars.get(i) + "\t" + values.get(i) + "\n");
			nedge ++;
		}
		
	}
	
	public void batchExec() throws SQLException, IOException{
		
		
		//file_template_factors_.close();
		//file_template_f2v_.close();
		
		/*
		String sql = "DROP TABLE TMP_LOAD_ IF EXISTS;";
		Config.debug("     SQL=" + sql);
		DB.execute(sql);

		sql = "CREATE TEXT TABLE TMP_LOAD_(fid INT, feature double, weight text, nvar int)";
		Config.debug("     SQL=" + sql);
		DB.execute(sql);
		
		sql = "SET TABLE TMP_LOAD_ SOURCE \"" + filename_template_factors_ + ";fs=\\t;encoding=UTF-8\"";
		Config.debug("     SQL=" + sql);
		DB.execute(sql);
		
		sql = "INSERT INTO template_factors_ SELECT * FROM TMP_LOAD_;";
		Config.debug("     SQL=" + sql);
		int update = DB.executeUpdate(sql);
		Config.info("    Actually inserted " + update + " factors.");
		
		sql = "DROP TABLE TMP_LOAD_ IF EXISTS;";
		Config.debug("     SQL=" + sql);
		DB.execute(sql);

		sql = "CREATE TEXT TABLE TMP_LOAD_(fid INT, pos INT, vid INT, label TEXT)";
		Config.debug("     SQL=" + sql);
		DB.execute(sql);
		
		sql = "SET TABLE TMP_LOAD_ SOURCE \"" + filename_template_f2v_ + ";fs=\\t;encoding=UTF-8\"";
		Config.debug("     SQL=" + sql);
		DB.execute(sql);
		
		sql = "INSERT INTO template_f2v_ SELECT * FROM TMP_LOAD_;";
		Config.debug("     SQL=" + sql);
		update = DB.executeUpdate(sql);
		
		Config.info("    Actually inserted " + update + " edges.");
		*/
	}
	
	
	
	
}
