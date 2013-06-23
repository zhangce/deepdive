package lda.Utils;

import org.kohsuke.args4j.Option;
/**
 * Parser for command line options.
 */
public class CommandOptions {

    @Option(name="-help", usage="Display command options.")
    public boolean showHelp = false;
	
    @Option(name="-e", aliases="-evidence", required=true, usage="Directory of input documents.")
    public String inputdir = "";
        
	@Option(name="-workdir", usage="Work dir for Elementary to do inference.")
    public String workdir = "/tmp";
	
    @Option(name="-alpha", usage="Parameter Alpha.")
    public double alpha = 0.01;

    @Option(name="-beta", usage="Parameter Beta.")
    public double beta = 0.01;

    @Option(name="-nTopics", usage="Number of topics.")
    public int ntopics = 50;
    
    @Option(name="-nSamples", usage="Number of samples to generate.")
    public int sample_step = 30;
    
    @Option(name="-db", usage="Host of DB.")
    public String db = "jdbc:postgresql://localhost:5432/postgres";

    @Option(name="-db_user", usage="User of DB.")
    public String db_user = "postgres";
    
    @Option(name="-db_password", usage="Password of DB.")
    public String db_password = "password";
    
    @Option(name="-db_schema", usage="Schema of DB.")
    public String db_schema = "deepdive_working_schema_lda";
    
}

