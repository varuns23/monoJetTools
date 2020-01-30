from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument('argv',nargs='*',help='accumlate undefined arguments',default=[])
parser.add_argument('--debug',help='enable debug option',action='store_true',default=False)

if parser.parse_known_args()[0].debug: import debug
