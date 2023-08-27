import os, sys
import argparse
from pathlib import Path
from .logger import Run


class ArgumentParser(argparse.ArgumentParser):
    def error(self, message):
        self.print_help(sys.stderr)
        self.exit(2, '\n%s: error: %s\n' % (self.prog, message))

if __name__ == "__main__":
    parser = ArgumentParser(
        prog = 'light sensor array',
    )
    parser.add_argument('-o', '--out', metavar='PATH', help="output folder", required=True)
    parser.add_argument('-w', '--ws', metavar='PATH', help="working directory", required=False, default="./")
    args = parser.parse_args(sys.argv[1:])
    os.chdir(args.ws)
    Run(Path(args.out).absolute())
