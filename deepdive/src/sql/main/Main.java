package sql.main;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;

import lda.Utils.CommandOptions;

import org.kohsuke.args4j.CmdLineException;
import org.kohsuke.args4j.CmdLineParser;

import sql.main.SQLCommandOptions;
import tuffy.db.RDB;
import tuffy.util.Config;
import tuffy.util.ExceptionMan;
import tuffy.util.StringMan;

public class Main {

	public static SQLCommandOptions parseCommand(String[] args){
		SQLCommandOptions opt = new SQLCommandOptions();
		CmdLineParser parser = new CmdLineParser(opt);
		try{
			parser.parseArgument(args);
			if(opt.showHelp){
				System.out.println("USAGE:");
	            parser.printUsage(System.out);
	            return null;
			}
		}catch(CmdLineException e){
			System.err.println(e.getMessage());
			System.out.println("USAGE:");
            parser.printUsage(System.out);
            Runtime.getRuntime().exit(1);
            return null;
		}

		return opt;
	}
	
	
	/**
	 * @param args
	 * @throws SQLException 
	 * @throws IOException 
	 */
	public static void main(String[] args) throws SQLException, IOException {
		
		SQLCommandOptions options = parseCommand(args);
		
		RDB db = new RDB(options.db, options.db_user, options.db_password);
		Config.dir_working = options.workdir;
		Config.db_schema = options.db_schema;
		Config.db_password = options.db_password;
		Config.db_username = options.db_user;
		
		//db.dropSchema(Config.db_schema);
		//db.execute("CREATE SCHEMA " + Config.db_schema + ";");
		//db.execute("SET search_path TO " + Config.db_schema + ";");
		
		db.setAutoCommit(false);
		
		// load sql
		String SELECT = options.sql_select;
		String FROM = options.sql_from;
		String WHERE = options.sql_where;
		String GROUPBY = options.sql_groupby;
		String USING = options.sql_using;
		
		String[] from_tables = FROM.split(",");
		ArrayList<String> id_tuples = new ArrayList<String>();
		ArrayList<String> table_names = new ArrayList<String>();
		ArrayList<String> target_schema = new ArrayList<String>();
		ArrayList<String> target_columns = new ArrayList<String>();
		for(String table : from_tables){
			String[] name_alias = table.split(" ");
			if(name_alias.length != 2){
				ExceptionMan.die("FROM Clause must contain alias for each table!");
			}
			id_tuples.add(name_alias[1] + ".vid AS _prov_var_id_" + name_alias[1] + " ");
			table_names.add(name_alias[1]);
			target_schema.add("_prov_var_id_" + name_alias[1] + " INT");
			target_columns.add("_prov_var_id_" + name_alias[1]);
		}
		
		String SQL_TO_RUN = "SELECT " + GROUPBY + "," + StringMan.join(",", id_tuples) + 
							"FROM " + FROM + "\n" + 
							"WHERE " + WHERE;
		
		// create new variables
		db.dropSequence("_seq_mat_query_variables");
		ResultSet rs = db.query("select last_value+1 from vid");
		while(rs.next()){
			db.execute("CREATE SEQUENCE _seq_mat_query_variables start " + rs.getInt(1));
		}
		rs.close();
		db.commit();
		// materialize SQL results (no group by)
		db.dropTable("_mat_query_rs");
		db.execute("CREATE TABLE _mat_query_rs (vid int default nextval('_seq_mat_query_variables'), " + "gb" + " TEXT, " + StringMan.join(", ", target_schema) + ")");
		db.execute("INSERT INTO _mat_query_rs(gb," + StringMan.join(", ", target_columns) + ") " + SQL_TO_RUN);	
		
		// materialize group by variables
		db.dropTable("_mat_group_rs");
		db.execute("CREATE TABLE _mat_group_rs (vid int default nextval('_seq_mat_query_variables'), tgroup TEXT, vgroup int[]);");
		db.execute("INSERT INTO _mat_group_rs(tgroup, vgroup) SELECT gb, array_agg(vid) FROM _mat_query_rs GROUP BY gb");	
		
		// add factors
		Integer largest_user_vid = -1;
		db.dropSequence("_seq_mat_query_factors");
		ResultSet rs2 = db.query("select last_value+1 from fid");
		while(rs2.next()){
			largest_user_vid = rs2.getInt(1);
			System.out.println(rs2.getInt(1));
			db.execute("CREATE SEQUENCE _seq_mat_query_factors start " + rs2.getInt(1));
		}
		rs2.close();
		db.commit();
		db.dropTable("_newfactors");
		db.execute("CREATE TABLE _newfactors (fid int default nextval('_seq_mat_query_factors'), vid int[], mid int, funcid int)" );
		db.execute("insert into _newfactors(vid, mid, funcid) select array[vid," + StringMan.join(",", target_columns) + "], 1, 4 from _mat_query_rs;");
		db.execute("insert into _newfactors(vid, mid, funcid) select array[vid] || vgroup, 1, " + USING + " from _mat_group_rs;");

		db.dropView("_allfactors");
		db.execute("CREATE VIEW _allfactors AS (SELECT * from _newfactors UNION SELECT * from existfactors)");

		
		db.execute("COPY (select fid, array_length(vid, 1), funcid, mid from _allfactors) to '" + options.workdir + "/factors.tsv" + "'");
		
		BufferedWriter bw = new BufferedWriter(new FileWriter(options.workdir + "/" + "_toload.tsv"));
		ResultSet rs3 = db.query("select fid, unnest(vid) from _allfactors;");
		Integer prevfid = -1;
		int poscounter = 0;
		while(rs3.next()){
			if(!prevfid.equals(rs3.getInt(1))){
				prevfid = rs3.getInt(1);
				poscounter = 0;
			}
			bw.write(rs3.getInt(1) + "\t" + rs3.getInt(2) + "\t" + poscounter + "\n");
			poscounter ++;
		}
		bw.close();
		
		db.dropTable("_fid_vid_pos");
		db.execute("CREATE TABLE _fid_vid_pos(fid int, vid int, pos int)");
		db.execute("COPY _fid_vid_pos FROM '" + options.workdir + "/" + "_toload.tsv'");
		
		db.setAutoCommit(false);
		
		ResultSet rs4 = db.query("select fid, vid, pos from _fid_vid_pos ORDER BY (vid, fid);");
		Integer prevvid = -1;
		String beforenfactor = "";
		ArrayList<String> afternfactor = new ArrayList<String>();
		Integer nfactor = 0;
		Integer fid, vid, pos;
		BufferedWriter bw2 = new BufferedWriter(new FileWriter(options.workdir + "/" + "variables.tsv"));
		while(rs4.next()){
			fid = rs4.getInt(1);
			vid = rs4.getInt(2);
			pos = rs4.getInt(3);
			
			if(!prevvid.equals(vid)){
				
				if(prevvid != -1){
					bw2.write(beforenfactor + "\t" + nfactor + "\t" + StringMan.join("\t", afternfactor)+"\t0\n");
				}
				
				prevvid = vid;
				
				if(vid < largest_user_vid){
					beforenfactor = vid + "\tCQS\t0\t1";
				}else{
					beforenfactor = vid + "\tRQG\t0\t10000";					
				}
				
				afternfactor = new ArrayList<String>();
				nfactor = 0;
			}
			
			afternfactor.add(fid + "\t0\t" + pos + "\t0");
			nfactor ++;
		}
		bw2.write(beforenfactor + "\t" + nfactor + "\t" + StringMan.join("\t", afternfactor)+"\t0\n");
		bw2.close();
		rs4.close();

		BufferedWriter bw3 = new BufferedWriter(new FileWriter(options.workdir + "/" + "models.tsv"));
		ResultSet rs5 = db.query("SELECT mid, value FROM model;");
		while(rs5.next()){
			bw3.write(rs5.getInt(1) + "\t" + rs5.getInt(2) + "\t" + 1 + "\n");
		}
		rs5.close();
		bw3.close();
		
		BufferedWriter bw4 = new BufferedWriter(new FileWriter(options.workdir + "/" + "group2vid.tsv"));
		ResultSet rs6 = db.query("SELECT vid, tgroup FROM _mat_group_rs");
		while(rs6.next()){
			bw4.write(rs6.getString(2) + "\t" + rs6.getInt(1) + "\n");
		}
		rs6.close();
		bw4.close();
		
		System.out.println(SQL_TO_RUN);
 		
	}

}


