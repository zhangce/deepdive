
import jeo

GLOBAL_WORD_ID = 0
GLOBAL_SENT_ID = 0

def parseConll(line):
    global GLOBAL_WORD_ID
    global GLOBAL_SENT_ID
    
    
    if(len(line) == 0):
        GLOBAL_SENT_ID = GLOBAL_SENT_ID + 1
        return None
    
    
    if line.rstrip() == '<s>':
        GLOBAL_SENT_ID = GLOBAL_SENT_ID + 1
        return None

    (word, pos, answer) = line.split(' ')
            
    GLOBAL_WORD_ID = GLOBAL_WORD_ID + 1

    return (GLOBAL_WORD_ID, GLOBAL_SENT_ID, word, pos, answer)

def parseConllTrain(inputtuples, (Words,)):
    for (line, ) in inputtuples:
        ss = parseConll(line)
        if ss == None: continue
        (wordid, sentid, word, pos, answer) = ss
        Words.insertTuple(wordid, sentid, word, pos)
        #if answer != 'O' and answer != 'B-NP' and answer != 'I-NP':
        #    answer = 'O'
        Words.setEvid("wordid", wordid, answer)

def parseConllTest(inputtuples, (Words,GroundTruth)):
    for (line, ) in inputtuples:
        ss = parseConll(line)
        if ss == None: continue
        (wordid, sentid, word, pos, answer) = ss
        #if answer != 'O' and answer != 'B-NP' and answer != 'I-NP':
        #    answer = 'O'
        Words.insertTuple(wordid, sentid, word, pos)
        GroundTruth.insertTuple(wordid, answer)

def unigram(inputtuples, (Words,)):
    
    lastword = ""
    lastsentid = ""
    
    #models = {}
    #for l in open('/tmp/models'):
    #    (a,b) = l.rstrip().split('\t')
    #    models[a[1:-1]] = float(b)
    
    fo = open('/tmp/output', 'w')
    
    for (wordid, word, pos, sentid) in inputtuples:
        
        if sentid != lastsentid:
            if lastword != "":
                END = "END-" + lastword
                for a in Words.domain:
                    sig = lastword + "-" + a
                    #fo.write(sig + " " +  END + " 0\n")
                    
        
            START = "START-%d" % wordid
            for a in Words.domain:
                sig = "%d-%s" % (wordid, a)
                #fo.write(START + " " +  sig + " 0\n")
                
        lastword = "%d" % wordid
        lastsentid = sentid
        
        
        jeo.AddDenseTemplateFactor(
            [Words.vars("wordid", wordid)],
            lambda (a,): (1, #"%f"%models["?Unigram-POS-" + pos + "LABEL-" + a] if "?Unigram-POS-" + pos + "LABEL-" + a in models else "0")
                          "?Unigram-POS-" + pos + "LABEL-" + a)
        )

        c = """
        jeo.AddDenseTemplateFactor(
            [Words.vars("wordid", wordid)],
            lambda (a,): (1, #"%f"%models["?Unigram-WORD-" + word + "LABEL-" + a] if "?Unigram-WORD-" + word + "LABEL-" + a in models else "0")
                        "?Unigram-WORD-" + word + "LABEL-" + a)
        )
            """
            
        jeo.AddDenseTemplateFactor(
            [Words.vars("wordid", wordid)],
            lambda (a,): (1, "?Unigram-WORD-" + word + "LABEL-" + "POS" + pos + a)
        )

        #jeo.AddDenseTemplateFactor(
        #    [Words.vars("wordid", wordid)],
        #    lambda (a,): (1, "?Bias-" + a)
        #)

        b = """
        for a in Words.domain:
            
            w = 0
            weight = "?Unigram-POS-" + pos + "LABEL-" + a
            if weight in models:
                w = w + models[weight]
            
            weight = "?Unigram-WORD-" + word + "LABEL-" + a
            if weight in models:
                w = w + models[weight]
            
            sig = "%d#####%s" % (wordid, a)
            fo.write(sig + " %f\n" % w)
            """

    b = """
    if lastword != "":
        END = "END-" + lastword
        for a in Words.domain:
            sig = lastword + "-" + a
            fo.write(sig + " " +  END + " 0\n")
    """
        
    fo.close()

def bigram(inputtuples, (Words,)):
    
    #models = {}
    #for l in open('/tmp/models'):
    #    (a,b) = l.rstrip().split('\t')
    #    models[a[1:-1]] = float(b)
    
    fo = open('/tmp/output', 'a')
    
    for (id1, word1, id2, word2) in inputtuples:
                
        jeo.AddDenseTemplateFactor(
            [Words.vars("wordid", word1), Words.vars("wordid", word2)],
            lambda (a,b,): (1, #"%f"%models["?Bigram-"+a+"-"+b] if "?Bigram-"+a+"-"+b in models else "0"),
                            "?Bigram-"+a+"-"+b),
            cachable="Bigram", usecache=True
        )

        b = """
        for a1 in Words.domain:
            for a2 in Words.domain:
                
                w = 0
                weight = "?Bigram-"+a1+"-"+a2
                w = w + models[weight]
                
                sig1 = "%d#####%s" % (word1, a1)
                sig2 = "%d#####%s" % (word2, a2)
                #print sig1, sig2, 1
                fo.write(sig1 + " " +  sig2 + " %f\n" % w)
            """

    fo.close()

def skip(inputtuples, (Words,)):
    
    models = {}
    for l in open('/tmp/models'):
        (a,b) = l.rstrip().split('\t')
        models[a[1:-1]] = float(b)
    
    fo = open('/tmp/output', 'a')
    
    for (id1, word1, id2, word2) in inputtuples:
        
        jeo.AddDenseTemplateFactor(
            [Words.vars("wordid", word1), Words.vars("wordid", word2)],
            lambda (a,b,): (1, #"%f"%models["?Skip-"+a+"-"+b]),
                                    "?Skip-"+a+"-"+b),
            cachable="SKIP", usecache=True
        )
        
        for a1 in Words.domain:
            for a2 in Words.domain:
                w = 0
                weight = "?Skip-"+a1+"-"+a2
                #w = w + models[weight]
            
                sig1 = "%d#####%s" % (word1, a1)
                sig2 = "%d#####%s" % (word2, a2)
                fo.write(sig1 + " " +  sig2 + " %f\n" % w)

    fo.close()


def precision(inputtuples, NORELS):
    npred = 0
    ncorrect = 0
    for (wordid, ans, gt) in inputtuples:
        
        print wordid, gt, '###GIBBS###', ans
        
        npred = npred + 1
        
        if(ans == gt):
            ncorrect = ncorrect + 1
    
    print "[JYTHON]  Precision =", 1.0*ncorrect/npred











