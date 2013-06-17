package Main;

import com.sun.jna.Library;
import com.sun.jna.Native;

/**
 * We compile the library with 
 *     clang++ -dynamiclib ui/JavaInterface.cpp -o libElle.dylib
 * @author czhang
 */
public class ElementaryInterface {

	interface DLibrary extends Library {

	    DLibrary INSTANCE = (DLibrary) Native.loadLibrary("Elle", DLibrary.class);
	    
	    void _Z9init_javav();
	    
	    void _Z8setModelid(int pid, double parameter);
	    
	    void _Z12setModelMaskib(int pid, boolean isFixed);

	    void _Z26addEvidMultinomialVariableliiii(long vid, int init_value, int lowerbound, int upperbound, int nfactor);

	    void _Z27addQueryMultinomialVariableliiii(long vid, int init_value, int lowerbound, int upperbound, int nfactor);

	    void _Z9addFactorliii(long fid, int pid, int nvar, int aux);

	    void _Z7addEdgelli(long vid, long fid, int aux);
	    
	    void _Z7addFauxlii(long fid, int nfaux, int faux);
	    
	    //double _Z14getExpectationl(long ovid);

	    double _Z6getMaxl(long ovid);
	    
	    double _Z7getProbl(long ovid);
	    
	    void _Z12runInferencev();

	    void _Z11runLearningv();
	    
	    void _Z7printfgv();
	    
	    double _Z8getModeli(int pid);
	    
	    
	    
	}
	
	public long maxvid = 0;
	
	public void init_java(){
		DLibrary.INSTANCE._Z9init_javav();
	}
	
	public double getModel(int pid){
		return DLibrary.INSTANCE._Z8getModeli(pid);
	}
	
	public void setModel(int pid, double parameter){
		DLibrary.INSTANCE._Z8setModelid(pid, parameter);
	}
    
	public void setModelMask(int pid, boolean isFixed){
    	DLibrary.INSTANCE._Z12setModelMaskib(pid, isFixed);
    }

	public void addEvidMultinomialVariable(long vid, int init_value, int lowerbound, int upperbound, int nfactor){
    	DLibrary.INSTANCE._Z26addEvidMultinomialVariableliiii(vid, init_value, lowerbound, upperbound, nfactor);
    }

	public void addQueryMultinomialVariable(long vid, int init_value, int lowerbound, int upperbound, int nfactor){
    	DLibrary.INSTANCE._Z27addQueryMultinomialVariableliiii(vid, init_value, lowerbound, upperbound, nfactor);
    }

	public void addFactor(long fid, int pid, int nvar, int aux){
    	DLibrary.INSTANCE._Z9addFactorliii(fid, pid, nvar, aux);
    }

	public void addEdge(long vid, long fid, int sign){
    	DLibrary.INSTANCE._Z7addEdgelli(vid, fid, sign);
    }
	
	public void addFaux(long fid, int nfaux, int faux){
		DLibrary.INSTANCE._Z7addFauxlii(fid, nfaux, faux);
	}
    
	public double getMax(long ovid){
    	return DLibrary.INSTANCE._Z6getMaxl(ovid);
    }
	
	public double getMaxProb(long ovid){
    	return DLibrary.INSTANCE._Z7getProbl(ovid);
    }

	public void runInference(){
    	DLibrary.INSTANCE._Z12runInferencev();
    }

	public void runLearning(){
    	DLibrary.INSTANCE._Z11runLearningv();
    }
    
	public void printfg(){
    	DLibrary.INSTANCE._Z7printfgv();
    }

	
}
