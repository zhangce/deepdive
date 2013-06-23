package Utils;

public class Config {

	public static String TASK_DIR;
	
	public static String DB_DIR=".db";
	
	public static String WORK_DIR="tmp";
	
	public static void info(String str){
		System.out.println("[INFO]  " + str);
	}

	public static void debug(String str){
		System.out.println("[DEBUG] " + str);
	}
	
	public static void error(String str){
		System.err.println("[ERROR] " + str);
	}
	
	public static String getAbsPath(String path){
		if(path.startsWith("/")){
			return path;
		}else{
			return TASK_DIR + "/" + path;
		}
	}
	
	public static int nfile = 0;
	public static String getNextTmpFile(){
		nfile = nfile + 1;
		return WORK_DIR + "/" + nfile;
	}
	
}
