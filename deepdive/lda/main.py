# -*- coding: utf-8 -*-

import os
import random


class LDA:

    options = None

    def __init__(self, options):
        self.options = options

    def generate_factor_graph(self):
        DOCDIR = self.options.evid
        WORKDIR = self.options.workdir

        print(("|LDA| DOCDIR  = %s" % DOCDIR))
        print(("|LDA| WORKDIR = %s" % WORKDIR))

        words = {}
        docs = {}
        ntokens = 0

        # load files
        print(("|LDA| LOADING FILES FROM %s ..." % DOCDIR))
        for docid in os.listdir(DOCDIR):
            filename = DOCDIR + '/' + docid
            if docid not in docs:
                docs[docid] = 0
            for l in open(filename):
                for word in l.rstrip().split(' '):
                    if word not in words:
                        words[word] = 0
                    words[word] = words[word] + 1
                    docs[docid] = docs[docid] + 1
                    ntokens = ntokens + 1

        print(("|LDA|    # DISTINCT DOC = %d" % len(docs)))
        print(("|LDA|    # TOKENS       = %d" % ntokens))
        print(("|LDA|    # VOC WORDS    = %d" % len(words)))

        ##### generate factors
        docfactors = {}
        wordfactors = {}

        factorfile = WORKDIR + "/" + "factors.tsv"
        print(("|LDA| CREATING FACTORS TO %s ..." % factorfile))
        fo = open(factorfile, 'w')

        fid = 0
        #first print the first factor
        fo.write(('%d\t%d\t0\t0\n' % (0, 50)))

        fid = 1
        #then print word factors
        for word in words:
            fo.write(('%d\t%d\t0\t1\n' % (fid, 50)))
            wordfactors[word] = fid
            fid = fid + 1

        #then print doc factors
        for doc in docs:
            fo.write(('%d\t%d\t0\t2\n' % (fid, 50)))
            docfactors[doc] = fid
            fid = fid + 1

        print(("|LDA|    # FACTORS      = %d" % (fid + 1)))

        fo.close()

        ##### generate variables
        variablefile = WORKDIR + "/" + "variables.tsv"
        print(("|LDA| CREATING VARIABLES TO %s ..." % variablefile))
        fo = open(variablefile, 'w')

        # load files
        vid = 0
        print(("|LDA| LOADING FILES FROM %s ..." % DOCDIR))
        for docid in os.listdir(DOCDIR):
            filename = DOCDIR + '/' + docid
            for l in open(filename):
                for word in l.rstrip().split(' '):
                    fo.write('%d\tC\t1\t50\t3\t%d\t0\t0\t0\t%d\t0\t0\t0\t%d\t0\t0\t0\t%d\n'
                        % (vid, 0, docfactors[docid], wordfactors[word],
                        random.randint(1, 50)))
                    vid = vid + 1
        fo.close()
        print(("|LDA|    # VARIABLES    = %d" % (vid)))

        ##### generate models
        modelfile = WORKDIR + "/" + "models.tsv"
        print(("|LDA| CREATING MODELS TO %s ..." % modelfile))
        fo = open(modelfile, 'w')

        fo.write('0\t0.00001\n')
        fo.write('1\t0.00002\n')
        fo.write('2\t0.00003')
        fo.close()

