package Utils;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class DB {

	static public Connection conn;
	
	static public String connstr(){
		return "jdbc:hsqldb:file:" + Config.DB_DIR;
	}
	
	static public void init() throws SQLException{
		conn = DriverManager.getConnection("jdbc:hsqldb:file:" + Config.DB_DIR + ";hsqldb.default_table_type=cached;shutdown=true" , "SA", "");
		conn.createStatement().execute("SET DATABASE SQL SYNTAX PGS TRUE;");
		
		//conn.setTransactionIsolation(Connection.TRANSACTION_READ_COMMITTED);
		conn.setAutoCommit(false);
		
		//
		conn.createStatement().execute("SHUTDOWN;");
		
		conn = DriverManager.getConnection("jdbc:hsqldb:file:" + Config.DB_DIR + ";hsqldb.default_table_type=cached;shutdown=true" , "SA", "");
		conn.createStatement().execute("SET DATABASE SQL SYNTAX PGS TRUE;");
		
		conn.setAutoCommit(false);
		
		conn.createStatement().execute("DROP TABLE template_f2v_ IF EXISTS;");
		conn.createStatement().execute("CREATE TABLE template_f2v_ (fid INT, pos INT, vid INT, label TEXT);");
		
		conn.createStatement().execute("DROP TABLE template_factors_ IF EXISTS;");
		conn.createStatement().execute("CREATE TABLE template_factors_ (fid INT, feature double, weight text, nvar int);");
		
		conn.createStatement().execute("DROP TABLE densetemplate_f2v_ IF EXISTS;");
		conn.createStatement().execute("CREATE TABLE densetemplate_f2v_ (fid INT, pos INT, vid INT);");
		
		conn.createStatement().execute("DROP TABLE densetemplate_factors_ IF EXISTS;");
		conn.createStatement().execute("CREATE TABLE densetemplate_factors_ (fid INT, nvar INT, auxinfo LONGVARBINARY);");
		
		
		conn.createStatement().execute("DROP SEQUENCE VARSEQ IF EXISTS CASCADE;");
		conn.createStatement().execute("CREATE SEQUENCE VARSEQ;");
	}
	
	static public void execute(String sql) throws SQLException{
		conn.createStatement().execute(sql);
	}
	
	static public int executeUpdate(String sql) throws SQLException{
		return conn.createStatement().executeUpdate(sql);
	}

}
