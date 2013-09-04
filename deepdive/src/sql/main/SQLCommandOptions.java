package sql.main;

import org.kohsuke.args4j.Option;
/**
 * Parser for command line options.
 */
public class SQLCommandOptions {

    @Option(name="-help", usage="Display command options.")
    public boolean showHelp = false;
	        
	@Option(name="-workdir", usage="Work dir for Elementary to do inference.")
    public String workdir = "/tmp";
	    
    @Option(name="-nSamples", usage="Number of samples to generate.")
    public int nsamples = 30;
    
    @Option(name="-buffer_size_bytes", usage="Buffer size in bytes [DEFAULT=1GB].")
    public long buffer_size_in_bytes = 1L*1024*1024*1024;
    
    @Option(name="-page_size_bytes", usage="Page size in bytes [DEFAULT=4MB].")
    public int page_size_in_bytes = 4*1024*1024;
    
    @Option(name="-db", usage="Host of DB.")
    public String db = "jdbc:postgresql://localhost:5432/postgres";

    @Option(name="-db_user", usage="User of DB.")
    public String db_user = "postgres";
    
    @Option(name="-db_password", usage="Password of DB.")
    public String db_password = "password";
    
    @Option(name="-db_schema", usage="Schema of DB.")
    public String db_schema = "deepdive_working_schema_lda";
    
    @Option(name="-select", usage="SQL to execute.")
    public String sql_select = "";
    
    @Option(name="-from", usage="SQL to execute.")
    public String sql_from = "";
    
    @Option(name="-where", usage="SQL to execute.")
    public String sql_where = "";
    
    @Option(name="-groupby", usage="SQL to execute.")
    public String sql_groupby = "";
    
    @Option(name="-using", usage="SQL to execute.")
    public String sql_using = "";
    
}

