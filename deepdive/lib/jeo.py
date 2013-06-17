
from com.caucho.quercus.module import AbstractQuercusModule
from com.caucho.quercus import Quercus
from com.caucho.quercus.env import ConstStringValue
from com.caucho.quercus.script import QuercusScriptEngine
from com.caucho.quercus.script import QuercusScriptEngineFactory


from javax.script import ScriptContext
import javax.script.ScriptEngine
import javax.script.ScriptEngineManager
import javax.script.ScriptException

import itertools
import os
import sys
from com.ziclix.python.sql import zxJDBC

zxJDBC.autocommit = 0

import java.util.ArrayList as Arraylist

_jdbc_url = ""
_driver = "org.hsqldb.jdbcDriver"
_conn = None
_task_dir = ""

CACHE_DENSE_TEMPLATE_FACTOR = {}

def _init_factors(factors):
    
    import constant
    
    print "[JYTHON] Seting Factor Class =", factors
    
    constant._factors = factors


def AddTemplateFactor(vars, template):

    import constant
    
    domains = []
    vids = []
    for v in vars:
        domains.append(v.relation.domain)
    
        vids.append(v.relation.getVIDs(v.column, v.value))
    
    dict = {}

    for Ls in itertools.product(*domains):
        for (feature, weight) in template(Ls):

            if feature == 0: continue

            for Vs in itertools.product(*vids):
                constant._factors.addTemplateFactor(Arraylist(Vs), Arraylist(Ls), feature, weight)


def AddDenseTemplateFactorByID(vids, domains, template, cachable=False, usecache=False):
    
    
    
    import constant

    
    
    VSS = []
    LABELS = []
    FEASS = []
    WEIGHTS = []


    
    for Ls in itertools.product(*domains):
        (feature, weight) = template(Ls)
        LABELS.append(Arraylist(Ls))
        FEASS.append(feature)
        WEIGHTS.append(weight)
                    

    
    #for Vs in itertools.product(*vids):
    #    VSS.append(Arraylist(Vs))
    
    VSS.append(Arraylist(vids))
    
    constant._factors.addDenseTemplateFactor(Arraylist(VSS), Arraylist(LABELS), Arraylist(FEASS), Arraylist(WEIGHTS))


def AddDenseTemplateFactor(vars, template, cachable=None, usecache=False):
    
    global CACHE_DENSE_TEMPLATE_FACTOR
    
    import constant
    import inspect
    
    domains = []
    vids = []
    for v in vars:
        domains.append(v.relation.domain)
        vids.append(v.relation.getVIDs(v.column, v.value))
        
    domaintuple = ()
    fsig = ""
    hit = False
            
    if cachable != None and usecache == True:
        import inspect
                
        domaintuple = tuple(domains)
                         
        fsig = cachable
            
        if domaintuple in CACHE_DENSE_TEMPLATE_FACTOR:
            if fsig in CACHE_DENSE_TEMPLATE_FACTOR[domaintuple]:
                hit = True

    VSS = []
    LABELS = []
    WEIGHTS = []
    
    if hit:
        (LABELS, WEIGHTS) = CACHE_DENSE_TEMPLATE_FACTOR[domaintuple][fsig]
    else:
        for Ls in itertools.product(*domains):
            (feature, weight) = template(Ls)
            LABELS.append(Arraylist(Ls))
            WEIGHTS.append(weight)
    
        if cachable != None and usecache == True:
            if domaintuple not in CACHE_DENSE_TEMPLATE_FACTOR:
                CACHE_DENSE_TEMPLATE_FACTOR[domaintuple] = {}
            if fsig not in CACHE_DENSE_TEMPLATE_FACTOR[domaintuple]:
                CACHE_DENSE_TEMPLATE_FACTOR[domaintuple][fsig] = (LABELS, WEIGHTS)
            
                    
    for Vs in itertools.product(*vids):
        VSS.append(Arraylist(Vs))

    constant._factors.addDenseTemplateFactor(Arraylist(VSS), Arraylist(LABELS), Arraylist(WEIGHTS))

#def GetLambda

def _init_path(task_dir):
    global _task_dir
    
    _task_dir = task_dir

    print "[JYTHON] Seting Task Dir =", _task_dir
    
    sys.path.append(_task_dir)

def _conndb(jdbc_url):
        
    global _jdbc_url
    global _driver
    global _conn

    _jdbc_url = jdbc_url

    print "[JYTHON] Connecting with ConnString =", _jdbc_url

    _conn = zxJDBC.connect(_jdbc_url + ";hsqldb.default_table_type=cached;shutdown=true", "SA", "", _driver)
    print _conn

def _close():
    
    global _conn
    cursor = _conn.cursor(1)
    cursor.execute("SHUTDOWN;")
    #_conn.commit()
    _conn.close()

def _exec(cmd):
    exec cmd

def _my_lazy_cursor(cursor):
    while True:
        r = cursor.fetchone()
        if r== None:
            break
        yield r



def _callphp(input, funcname, rels, factors):

    ss = funcname.split('.')
    func = ss[-1]
    phpfile = ".".join(ss[0:-1])
    
    global _factors
        
    print "[JYTHON] Executing", func, "in file", phpfile
    print "[JYTHON]     Querying", input
    
    global _conn
    cursor = _conn.cursor(True)
    cursor.execute(input)
    
    quercus = Quercus()
    #quercus.setIni("error_reporting", "0");
    quercus.init();
    quercus.start();
    phpengine = QuercusScriptEngine(QuercusScriptEngineFactory(), quercus);

    import constant
    JOEWRAPPEROBJ = constant.AAAAA()
    
    CUR = _my_lazy_cursor(cursor);
    phpengine.getContext().setAttribute("_CUR", CUR, ScriptContext.ENGINE_SCOPE)
    phpengine.getContext().setAttribute("_B", rels, ScriptContext.ENGINE_SCOPE)
    phpengine.getContext().setAttribute("FACTORS", factors, ScriptContext.ENGINE_SCOPE)
    
    phpengine.eval("<?php include ('lib/jeo.php'); include('" + phpfile + "'); " + func + "($_CUR, $_B) ?>")
    
    _conn.commit()
    
    
def _callfunc(input, funcname, rels):
    
    #print rels
    global _factors
    
    ss = funcname.split('.')
    for i in range(1, len(ss)):
        libname = ".".join(ss[0:i])
        print "[JYTHON] Importing", libname
        exec "import " + libname
            
    print "[JYTHON] Executing", funcname
    print "[JYTHON]     Querying", input
    
    global _conn
    cursor = _conn.cursor(1)
    cursor.execute(input)
    eval(funcname + "(" + "_my_lazy_cursor(cursor)" + ", rels)")

    _conn.commit()




