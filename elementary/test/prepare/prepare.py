# -*- coding: utf-8 -*-

from optparse import OptionParser

from lda.main import LDA

def parse_arg():
    parser = OptionParser()
    parser.add_option("-a", "--app", dest="app",
                        help="Application to run", metavar="[LDA]")
    parser.add_option("-e", "--evid", dest="evid",
                        help="Path to evidence files", metavar="FILE/FOLDER")

    parser.add_option("-w", "--workdir", dest="workdir",
                        help="Working Directory", metavar="FOLDER")

    (options, args) = parser.parse_args()

    if not options.app:
        parser.error('-a (--app) not given')

    if not options.evid:
        parser.error('-e (--evid) not given')

    if not options.workdir:
        options.workdir = "/tmp/deepdive"

    return options


def main():
    options = parse_arg()

    ui = None

    if options.app == 'lda':
        ui = LDA(options)

    ui.generate_factor_graph()


#if __name__ == "__main__":
main()