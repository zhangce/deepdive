package Main;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Random;

import org.apache.commons.lang3.SerializationUtils;

//import Main.ElementaryInterface;
import Utils.Config;
import Utils.DB;
import DStruct.Factor;
import DStruct.Relation;

/*
public class BridgeToElly {
	
	Random rand = new Random();
	
	ElementaryInterface elly = new ElementaryInterface();
	
	HashMap<String, Integer> weight2pid = new HashMap<String, Integer>();
	
	public void dumpModel() throws SQLException{
		
		for(String weight : weight2pid.keySet()){
			Double modelvalue = elly.getModel(weight2pid.get(weight));
			Config.debug("MODEL:    FEATURE [" + weight + "]  ~> " + modelvalue);
		}
		
	}
	
	public void addFactors() throws SQLException{
		
		Integer cpid = -1;
		weight2pid = new HashMap<String, Integer>();
		
		Config.info("Inserting factors into Elementary...");
		
		String sql = "SELECT * FROM " + Factor.densetable_template_factors + " ORDER BY fid" ;
		Config.debug("    SQL=" + sql);
		ResultSet rs = DB.conn.createStatement().executeQuery(sql);
		
		while(rs.next()){
			
			Integer fid = rs.getInt(1);
			Integer nvar = rs.getInt(2);
			
			// these two can be much faster
			HashMap<String, byte[]> aux = 
					(HashMap<String, byte[]>) SerializationUtils.deserialize(rs.getBytes(3));
			
			//ArrayList<ArrayList<String>> VALUESS = (ArrayList<ArrayList<String>>) SerializationUtils.deserialize(aux.get("values"));
			//ArrayList<Double> FEATURES = (ArrayList<Double>) SerializationUtils.deserialize(aux.get("features"));
			
			ArrayList<String> WEIGHTS = (ArrayList<String>) SerializationUtils.deserialize(aux.get("weights"));
			ArrayList<Integer> weights = new ArrayList<Integer>();
			for(String weight : WEIGHTS){
				Double initweight = 0.1;
				Integer pid = 0;
				
				if(weight.startsWith("?")){
					
					String remain = weight.replaceFirst("\\?", "");
					
					try{
						initweight = Double.parseDouble(remain);
						String signature = weight + "~~~~FID=" + fid;	//for unfixed weight, we associate them with each factor
						if(weight2pid.containsKey(signature)){
							pid = weight2pid.get(signature);
						}else{
							cpid ++;
							pid = cpid;
							weight2pid.put(signature, pid);
							elly.setModel(pid, initweight);
							elly.setModelMask(pid, false);
						}
					}catch(Exception e){
						initweight = 0.1;
						if(weight2pid.containsKey(weight)){
							pid = weight2pid.get(weight);
						}else{
							cpid ++;
							pid = cpid;
							weight2pid.put(weight, pid);
							elly.setModel(pid, initweight);
							elly.setModelMask(pid, false);
						}
					}
					
					
				}else{
					try{
						initweight = Double.parseDouble(weight);
						if(weight2pid.containsKey(weight)){
							pid = weight2pid.get(weight);
						}else{
							cpid ++;
							pid = cpid;
							weight2pid.put(weight, pid);
							elly.setModel(pid, initweight);
							elly.setModelMask(pid, true);
						}
						
					}catch(Exception e){
						Config.error("You can only use string weights for unknown factors!");
						Runtime.getRuntime().exit(-1);
					}
				}
				
				weights.add(pid);
			}
			
			//System.out.println(weights.size() + "\t" + weights);
			
			elly.addFactor(fid.longValue(), 0, nvar, weights.size());
			for(int i=0;i<weights.size();i++){
				elly.addFaux(fid.longValue(), i, weights.get(i));
			}
		}
		
	}
	
	
	public void addFactors_sparse() throws SQLException{
		
		Integer cpid = -1;
		weight2pid = new HashMap<String, Integer>();
		
		Config.info("Inserting factors into Elementary...");
		
		String sql = "SELECT * FROM " + Factor.table_template_factors + " ORDER BY fid" ;
		Config.debug("    SQL=" + sql);
		ResultSet rs = DB.conn.createStatement().executeQuery(sql);
		
		while(rs.next()){
			
			Integer fid = rs.getInt(1);
			Double feature = rs.getDouble(2);
			String weight = rs.getString(3);
			Integer nvar = rs.getInt(4);
			
			Double initweight = 0.1;
			Integer pid = 0;
			
			if(weight.startsWith("?")){
				
				String remain = weight.replaceFirst("\\?", "");
				
				try{
					initweight = Double.parseDouble(remain);
					String signature = weight + "~~~~FID=" + fid;	//for unfixed weight, we associate them with each factor
					if(weight2pid.containsKey(signature)){
						pid = weight2pid.get(signature);
					}else{
						cpid ++;
						pid = cpid;
						weight2pid.put(signature, pid);
						elly.setModel(pid, initweight);
						elly.setModelMask(pid, false);
					}
				}catch(Exception e){
					initweight = 0.1;
					if(weight2pid.containsKey(weight)){
						pid = weight2pid.get(weight);
					}else{
						cpid ++;
						pid = cpid;
						weight2pid.put(weight, pid);
						elly.setModel(pid, initweight);
						elly.setModelMask(pid, false);
					}
				}
				
				
			}else{
				try{
					initweight = Double.parseDouble(weight);
					if(weight2pid.containsKey(weight)){
						pid = weight2pid.get(weight);
					}else{
						cpid ++;
						pid = cpid;
						weight2pid.put(weight, pid);
						elly.setModel(pid, initweight);
						elly.setModelMask(pid, true);
					}
					
				}catch(Exception e){
					Config.error("You can only use string weights for unknown factors!");
					Runtime.getRuntime().exit(-1);
				}
			}
						
			//System.out.println("FACTOR F" + fid + " ->" + feature + " NV=" + nvar + "   PID=" + pid + "   INITWEIGHT=" + initweight);
			
				// TODO: CHANGE IT TO DOUBLE
			elly.addFactor(fid.longValue(), pid, nvar, feature.intValue());
			
		}
		
		rs.close();
	
	}
	
	public void addVariables() throws SQLException{
		
		Config.info("Extracting variables that we are interested in.");
		
		DB.execute("DROP TABLE interesting_variables_tmp_ IF EXISTS CASCADE");
		DB.execute("CREATE TABLE interesting_variables_tmp_ (vid int, nfactor int)");
		
		Config.debug("    SQL=INSERT INTO interesting_variables_tmp_ SELECT vid, count(DISTINCT fid) FROM " + Factor.densetable_template_f2v_ + " GROUP BY vid");
		DB.execute("INSERT INTO interesting_variables_tmp_ SELECT vid, count(DISTINCT fid) FROM " + Factor.densetable_template_f2v_ + " GROUP BY vid");
	
		Config.debug("    SQL=CREATE INDEX idx_interesting_variables_tmp_ ON interesting_variables_tmp_(vid);");
		DB.execute("CREATE INDEX idx_interesting_variables_tmp_ ON interesting_variables_tmp_(vid);");
		
		Config.debug("    SQL=CREATE INDEX idx_template_v2f_tmp_ ON " + Factor.densetable_template_f2v_ + "(vid);");
		DB.execute("CREATE INDEX idx_template_v2f_tmp_ ON " + Factor.densetable_template_f2v_ + "(vid);");
		
		for(String rel : Relation.relations.keySet()){
			
			DB.execute("DROP TABLE tmp_care_in_this_rel IF EXISTS CASCADE");
			DB.execute("CREATE TABLE tmp_care_in_this_rel (vid int)");
			
			Config.debug("    SQL=INSERT INTO tmp_care_in_this_rel " + "SELECT DISTINCT t0.id FROM " + rel + " t0, interesting_variables_tmp_ t1 "+
					 " WHERE t0.id=t1.vid");
			DB.execute("INSERT INTO tmp_care_in_this_rel " + "SELECT DISTINCT t0.id FROM " + rel + " t0, interesting_variables_tmp_ t1 "+
					 " WHERE t0.id=t1.vid");
			
			Config.debug("    SQL=CREATE INDEX idx_tmp_care_in_this_rel ON tmp_care_in_this_rel(vid);");
			DB.execute("CREATE INDEX idx_tmp_care_in_this_rel ON tmp_care_in_this_rel(vid);");
			
			Relation relation = Relation.relations.get(rel);
			
			String sql = "SELECT t0.*, t1.nfactor FROM " + rel + " t0, interesting_variables_tmp_ t1 "+
						 " WHERE t0.id=t1.vid";
			Config.debug("    SQL=" + sql);
			ResultSet rs = DB.conn.createStatement().executeQuery(sql);
			
			int domainsize = relation.domain.size();
			
			boolean check = true;
			while(rs.next()){
				if(check){
					if(domainsize == 0){
						Config.error("You need to define the domain for relation " + rel + " because you are using tuples from it as variables!");
						Runtime.getRuntime().exit(-1);
					}
				}
				check = false;
				
				Integer ID = rs.getInt("id");
				String varvalue = rs.getString("varvalue_");
				Integer nfactor = rs.getInt("nfactor");
				
				if(rs.getBoolean("isevid_")){
					elly.addEvidMultinomialVariable(ID, relation.domainvalue2index.get(varvalue), 0, domainsize - 1, nfactor);
					//System.out.println("EVID V" + ID + " 0~" + domainsize + " @ " + relation.domainvalue2index.get(varvalue) + " NF=" + nfactor);
				}else{
					elly.addQueryMultinomialVariable(ID, rand.nextInt(domainsize), 0, domainsize - 1, nfactor);
					//System.out.println("QUERY V" + ID + " 0~" + domainsize + " NF=" + nfactor);
				}
			}
			rs.close();
			
			sql = "SELECT t0.fid, t0.vid FROM " + Factor.densetable_template_f2v_ + " t0, tmp_care_in_this_rel t1 WHERE t0.vid=t1.vid ORDER BY t0.fid, t0.pos;";
			Config.debug("    SQL=" + sql);
			ResultSet rs2 = DB.conn.createStatement().executeQuery(sql);
			while(rs2.next()){
				
				Integer FID = rs2.getInt("fid");
				Integer VID = rs2.getInt("vid");
				//String label = rs2.getString("label");
				
				//System.out.println("EDGE V" + VID + "   F" + FID + "   LABEL=" + relation.domainvalue2index.get(label));
				elly.addEdge(VID, FID, relation.domain.size());
				
			}
			rs2.close();
	
		}
	}
	
	public void dumpRS() throws SQLException{
		
		Config.info("Extracting variables that we are interested in.");
		
		DB.execute("DROP TABLE interesting_variables_tmp_ IF EXISTS CASCADE");
		DB.execute("CREATE TABLE interesting_variables_tmp_ (vid int, nfactor int)");
		
		Config.debug("    SQL=INSERT INTO interesting_variables_tmp_ SELECT vid, count(DISTINCT fid) FROM " + Factor.densetable_template_f2v_ + " GROUP BY vid");
		DB.execute("INSERT INTO interesting_variables_tmp_ SELECT vid, count(DISTINCT fid) FROM " + Factor.densetable_template_f2v_ + " GROUP BY vid");
	
		Config.debug("    SQL=CREATE INDEX idx_interesting_variables_tmp_ ON interesting_variables_tmp_(vid);");
		DB.execute("CREATE INDEX idx_interesting_variables_tmp_ ON interesting_variables_tmp_(vid);");
				
		for(String rel : Relation.relations.keySet()){
			
			DB.execute("DROP TABLE tmp_care_in_this_rel IF EXISTS CASCADE");
			DB.execute("CREATE TABLE tmp_care_in_this_rel (vid int)");
			
			//Config.debug("    SQL=INSERT INTO tmp_care_in_this_rel " + "SELECT DISTINCT t0.id FROM " + rel + " t0, interesting_variables_tmp_ t1 "+
			//		 " WHERE t0.id=t1.vid");
			DB.execute("INSERT INTO tmp_care_in_this_rel " + "SELECT DISTINCT t0.id FROM " + rel + " t0, interesting_variables_tmp_ t1 "+
					 " WHERE t0.id=t1.vid");
			
			//Config.debug("    SQL=CREATE INDEX idx_tmp_care_in_this_rel ON tmp_care_in_this_rel(vid);");
			DB.execute("CREATE INDEX idx_tmp_care_in_this_rel ON tmp_care_in_this_rel(vid);");
			
			Relation relation = Relation.relations.get(rel);
			
			String sql = "SELECT t0.*, t1.nfactor FROM " + rel + " t0, interesting_variables_tmp_ t1 "+
						 " WHERE t0.id=t1.vid";
			Config.debug("    SQL=" + sql);
			ResultSet rs = DB.conn.createStatement().executeQuery(sql);
			
			int domainsize = relation.domain.size();
			
			boolean check = true;
			while(rs.next()){
				if(check){
					if(domainsize == 0){
						Config.error("You need to define the domain for relation " + rel + " because you are using tuples from it as variables!");
						Runtime.getRuntime().exit(-1);
					}
				}
				check = false;
				
				Integer ID = rs.getInt("id");
				String varvalue = rs.getString("varvalue_");
				Integer nfactor = rs.getInt("nfactor");
				
				if(rs.getBoolean("isevid_")){
					//elly.addEvidMultinomialVariable(ID, relation.domainvalue2index.get(varvalue), 0, domainsize - 1, nfactor);
					//System.out.println("EVID V" + ID + " 0~" + domainsize + " @ " + relation.domainvalue2index.get(varvalue) + " NF=" + nfactor);
				}else{
					String rrr = "";
					rrr += elly.getMaxProb(ID) + "\t";
					rrr += relation.domain.get(((int)elly.getMax(ID)))+"\t";
					rrr += relation.relname + "(";
					for(String col : relation.columns){
						rrr += "\"" + rs.getString(col).replace("\"", "\\\"") + "\",";
					}
					rrr += ")";
					//Config.debug("INFRS:    " + rrr);
					relation.setEvid("id", ID, relation.domain.get(((int)elly.getMax(ID))), elly.getMaxProb(ID));
				}
			}
			rs.close();
	
		}
	}

}
*/

