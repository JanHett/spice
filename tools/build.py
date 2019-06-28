import argparse
import subprocess
import os
import sys

class colors:
    HEADER = '\033[95m'
    OK = '\033[94m'
    SUCCESS = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    ENDC = '\033[0m'

TARGETS = [
    'spice',
    'tests',
    'spice_coverage'
    ]

def p_success(message):
    print(f"{colors.SUCCESS}{message}{colors.ENDC}")

def p_warning(message):
    print(f"{colors.WARNING}{message}{colors.ENDC}")

def p_fail(message):
    print(f"{colors.FAIL}{message}{colors.ENDC}")

def p_format(message, *format):
    print(f"{''.join(format)}{message}{colors.ENDC}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Build the spice library.')
    parser.add_argument('root',
        default='.',
        help='The root directory of the project')
    parser.add_argument('build_type', metavar='build-type',
        default='Debug',
        choices=[
            'Debug',
            'Release'
            ],
        help='-DCMAKE_BUILD_TYPE')
    parser.add_argument('--target', '-t',
        default=TARGETS,
        nargs='+',
        choices=TARGETS,
        help='[ UNIMPLEMENTED ] Build the specified targets')
    parser.add_argument('--run', '-r',
        choices=TARGETS,
        help='Run the specified target after building it. Implies that the '
        'target should be built. If it wasn\'t already included, it will be '
        'appended to the list passed to `--target`.')
    parser.add_argument('--run-args',
        help='Add the arguments to the call to execute the built program. has '
        'to be passed as a string.')
    parser.add_argument('--build-documentation', '-d',
        action='store_true',
        help='Build the documentation.')

    args = parser.parse_args()

    build_dir = os.path.abspath(os.path.join(args.root, 'build'))

    os.makedirs(build_dir, exist_ok=True)

    # build it
    cmake = subprocess.run(["cmake",
        f"-DCMAKE_BUILD_TYPE={args.build_type}",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        '..'],
        cwd=build_dir)

    if cmake.returncode == 0:
        print(f"{colors.SUCCESS}Finished building targets "
            f"[{', '.join(args.target)}].{colors.ENDC}")

        # symlink compilation DB
        subprocess.run(["ln",
            "-s",
            "./compile_commands.json",
            "../compile_commands.json"],
            cwd=build_dir)
    else:
        print(f"{colors.FAIL}Building targets [{', '.join(args.target)}] "
            f"failed. Aborting.{colors.ENDC}")
        sys.exit(cmake.returncode)


    # make it
    make = subprocess.run(["make"], cwd=build_dir)

    if 'spice_coverage' in args.target:
        make = subprocess.run(["make", "spice_coverage"], cwd=build_dir)

    if make.returncode == 0:
        print(f"{colors.SUCCESS}Finished making targets "
            f"[{', '.join(args.target)}].{colors.ENDC}")
    else:
        print(f"{colors.FAIL}Making targets [{', '.join(args.target)}] failed. "
            f"Aborting.{colors.ENDC}")
        sys.exit(make.returncode)

    # build the docs
    if args.build_documentation:
        doc_dir = os.path.abspath(os.path.join(args.root, 'doc'))

        doxygen = subprocess.run([
            "../external/m.css/documentation/doxygen.py",
            # "--debug",
            "Doxyfile-mcss"
            ], cwd=doc_dir)
        if make.returncode == 0:
            p_success("Doxygen finished.")
        else:
            p_fail("Doxygen failed.")

    if args.run:
        if args.run_args:
            run_args = args.run_args.split()
        else:
            run_args = []
        run_cmd = [f'./{args.run}', *run_args]
    
        subprocess.run(run_cmd, cwd=build_dir)
