import argparse
import subprocess
import os

TARGETS = [
    'spice',
    'test'
    ]

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

    args = parser.parse_args()

    build_dir = os.path.abspath(os.path.join(args.root, 'build'))

    os.makedirs(build_dir, exist_ok=True)

    # build it
    subprocess.run(["cmake", f"-DCMAKE_BUILD_TYPE={args.build_type}", '..'],
        cwd=build_dir)

    if args.run:
        if args.run_args:
            run_args = args.run_args.split()
        else:
            run_args = []
        subprocess.run([f'./{args.run}'].extend(run_args), cwd=build_dir)
