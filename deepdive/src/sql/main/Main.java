package sql.main;

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
	 */
	public static void main(String[] args) {
		
		SQLCommandOptions options = parseCommand(args);
		
		RDB db = new RDB(options.db, options.db_user, options.db_password);
		Config.dir_working = options.workdir;
		Config.db_schema = options.db_schema;
		Config.db_password = options.db_password;
		Config.db_username = options.db_user;
		
		db.dropSchema(Config.db_schema);
		db.execute("CREATE SCHEMA " + Config.db_schema + ";");
		db.execute("SET search_path TO " + Config.db_schema + ";");
		
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
		for(String table : from_tables){
			String[] name_alias = table.split(" ");
			if(name_alias.length != 2){
				ExceptionMan.die("FROM Clause must contain alias for each table!");
			}
			id_tuples.add(name_alias[1] + ".__id AS __prov_var_id_" + name_alias[1] + " ");
			table_names.add(name_alias[0]);
		}
		
		String SQL_TO_RUN = "SELECT " + SELECT + ",\n" +
							"       " + StringMan.join(",", id_tuples) + 
							"FROM " + FROM + "\n" + 
							"WHERE " + WHERE;
		
		System.out.println(SQL_TO_RUN);

	}

}


