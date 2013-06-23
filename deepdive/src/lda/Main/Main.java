package lda.Main;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;

import org.kohsuke.args4j.CmdLineException;
import org.kohsuke.args4j.CmdLineParser;
import org.postgresql.PGConnection;

import tuffy.db.RDB;
import tuffy.util.Config;
import tuffy.util.FileMan;
import tuffy.util.StringMan;
import tuffy.util.Timer;

import lda.Utils.CommandOptions;

public class Main {
	
	public static CommandOptions parseCommand(String[] args){
		CommandOptions opt = new CommandOptions();
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
	
	public static long load(String input_dir, String work_dir, RDB db) throws IOException, SQLException{
		BufferedWriter fo_docids = new BufferedWriter(new FileWriter(work_dir + "/" + "lda_tmp_docids.tsv"));
		BufferedWriter fo_tokens = new BufferedWriter(new FileWriter(work_dir + "/" + "lda_tmp_tokens.tsv"));
		
		String file;
		String line;
		File folder = new File(input_dir);
		File[] listOfFiles = folder.listFiles(); 
		long docid = 0; 
		long tokenid = 0;
		
		System.out.println("[LDA] Loading input files from " + input_dir  + "...");
		Timer t = new Timer();
		for (int i = 0; i < listOfFiles.length; i++) {
			if (listOfFiles[i].isFile()){
				file = listOfFiles[i].getName();
				docid ++;
				fo_docids.write(docid + "\t" + file + "\n");
				BufferedReader fin = new BufferedReader(new FileReader(input_dir + "/" + file));
				while( (line = fin.readLine()) != null){
					line = line.replaceAll("\t", " ");
					for(String word : line.split(" ")){
						if(word.length() > 0){
							fo_tokens.write(tokenid + "\t" + docid + "\t" + word + "\n");
						}
					}
				}
		    }
		}
		System.out.println("[LDA]       " + t.elapsedSeconds() + " seconds");
		
		fo_docids.close();
		fo_tokens.close();
		
		db.dropTable("lda_docids");
		db.dropTable("lda_tokens");
		db.execute("CREATE TABLE lda_docids (docid BIGINT, filename TEXT);");
		db.execute("CREATE TABLE lda_tokens (tokenid BIGINT, docid BIGINT, word TEXT);");		
		db.commit();
		
		System.out.println("[LDA] Loading docids table from " + work_dir + "/" + "lda_tmp_docids.tsv" + "...");
		t.resetClock();
		FileInputStream in = new FileInputStream(work_dir + "/" + "lda_tmp_docids.tsv");
		PGConnection con = (PGConnection) db.getConnection();
		con.getCopyAPI().copyIn("COPY lda_docids FROM STDIN", in);
		in.close();
		System.out.println("[LDA]       " + t.elapsedSeconds() + " seconds");
		
		t.resetClock();
		System.out.println("[LDA] Loading tokens table from " + work_dir + "/" + "lda_tmp_tokens.tsv" + "...");
		in = new FileInputStream(work_dir + "/" + "lda_tmp_tokens.tsv");
		con.getCopyAPI().copyIn("COPY lda_tokens FROM STDIN", in);
		in.close();
		System.out.println("[LDA]       " + t.elapsedSeconds() + " seconds");
		
		db.analyze("lda_docids");
		db.analyze("lda_tokens");
		
		return docid;
		
	}
	
	public static void assign_factorid_to_words(long currentdocid, String work_dir, RDB db) throws IOException, SQLException{
		
		Timer t = new Timer();
		System.out.println("[LDA] SQL: Assign factor id to distinct words...");
		
		db.dropSequence("lda_wordid");
		db.execute("CREATE SEQUENCE lda_wordid START " + (currentdocid + 1) + ";");
		
		db.dropTable("lda_words");
		db.execute("CREATE TABLE lda_words AS SELECT word, nextval('lda_wordid') wordid FROM (SELECT DISTINCT word FROM lda_tokens) nt;");
		
		System.out.println("[LDA]       " + t.elapsedSeconds() + " seconds");
		
		t.resetClock();
		System.out.println("[LDA] SQL: Create index on words...");
		db.dropIndex("lda_index_wordid_word");
		db.execute("CREATE INDEX lda_index_wordid_word ON lda_words(word);");
		db.analyze("lda_words");
		System.out.println("[LDA]       " + t.elapsedSeconds() + " seconds");
		
		
	}
	
	public static void generate_factor_file(CommandOptions opt, String work_dir, RDB db) throws IOException, SQLException{
		
		Timer t = new Timer();
		System.out.println("[LDA] SQL: Create factor files to " + work_dir + "/" + "factors.tsv" + "...");
		
		BufferedWriter fo_factors = new BufferedWriter(new FileWriter(work_dir + "/" + "factors.tsv"));
		
		fo_factors.write("0\t" + opt.ntopics + "\t0\t-1\n");
		long nfactor = 1;
		
		ResultSet rs = db.query("SELECT docid FROM lda_docids ORDER BY docid;");
		while(rs.next()){
			nfactor++;
			fo_factors.write(rs.getLong(1) + "\t" + opt.ntopics + "\t0\t2\n");
		}
		rs.close();
		
		long nword = 0;
		ResultSet rs2 = db.query("SELECT wordid FROM lda_words ORDER BY wordid;");
		while(rs2.next()){
			nfactor++;
			fo_factors.write(rs2.getLong(1) + "\t" + opt.ntopics + "\t0\t3\n");
			nword = nword + 1;
		}
		rs2.close();
		
		fo_factors.close();
		System.out.println("[LDA]       # factor = " + nfactor);
		System.out.println("[LDA]       " + t.elapsedSeconds() + " seconds");
		
		System.out.println("[LDA] SQL: Create model files to " + work_dir + "/" + "models.tsv" + "...");
		t.resetClock();
		
		BufferedWriter fo_models = new BufferedWriter(new FileWriter(work_dir + "/" + "models.tsv"));
		fo_models.write("1\t" + (opt.beta * nword) + "\t1\n");
		fo_models.write("2\t" + (opt.beta) + "\t1\n");
		fo_models.write("3\t" + (opt.alpha) + "\t1\n");
		fo_models.close();
		
		System.out.println("[LDA]       # model = " + 3);
		System.out.println("[LDA]       " + t.elapsedSeconds() + " seconds");
	}
	
	public static void generate_variable_file(CommandOptions opt, String work_dir, RDB db) throws IOException, SQLException{
		
		Timer t = new Timer();
		System.out.println("[LDA] SQL: Create factor files to " + work_dir + "/" + "variables.tsv" + "...");
		
		BufferedWriter fo_variables= new BufferedWriter(new FileWriter(work_dir + "/" + "variables.tsv"));
		
		long nvar = 0;
		ResultSet rs = db.query("SELECT t0.tokenid, t0.docid, t1.wordid FROM lda_tokens t0, lda_words t1 WHERE t0.word=t1.word;");
		while(rs.next()){
			nvar ++;
			fo_variables.write(rs.getLong(1) + "\tCQS\t0\t" + opt.ntopics + "\t3\t" +
								0 + "\t" + "0\t0\t0\t" + 
								rs.getLong(2) + "\t" + "0\t0\t0\t" + 
								rs.getLong(3) + "\t" + "0\t0\t0\t" + 
								"0\n");
		}
		System.out.println("[LDA]       # var = " + nvar);
		rs.close();
		fo_variables.close();
		
		System.out.println("[LDA]       " + t.elapsedSeconds() + " seconds");
	}

	public static void main(String[] args) throws IOException, SQLException {
		
		CommandOptions options = parseCommand(args);
		
		RDB db = new RDB(options.db, options.db_user, options.db_password);
		Config.dir_working = options.workdir;
		Config.db_schema = options.db_schema;
		Config.db_password = options.db_password;
		Config.db_username = options.db_user;
		
		db.dropSchema(Config.db_schema);
		db.execute("CREATE SCHEMA " + Config.db_schema + ";");
		db.execute("SET search_path TO " + Config.db_schema + ";");
		
		db.setAutoCommit(false);
		
		long currentdocid = load(options.inputdir, options.workdir, db);
		assign_factorid_to_words(currentdocid, options.workdir, db);
		
		generate_variable_file(options, options.workdir, db);
		generate_factor_file(options, options.workdir, db);
	}

}
