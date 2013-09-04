for l in open('train', 'r'):
    try:(a,b,answer) = l.rstrip().split(' ')
    except:
        print ""
        continue
    if answer != 'O' and answer != 'B-NP' and answer != 'I-NP': answer = 'O'
    print " ".join([a,b,answer])
