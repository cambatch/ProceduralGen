import subprocess
import sys
import os

def cmake_build(build_dir, build_type, generator):
    curr_dir = os.getcwd()

    if not os.path.exists(build_dir):
        os.mkdir(build_dir)

    os.chdir(build_dir)

    if sys.platform.startswith('win32'):
        if generator == 'Ninja':
            cmake_cmd = ['cmake', '-S', '..', '-B', build_type, '-G', 'Ninja']
            build_cmd = ['cmake', '--build', build_type]
        elif generator == 'Visual Studio':
            cmake_cmd = ['cmake', '..', '-G', 'Visual Studio 17 2022']
            build_cmd = ['cmake', '--build', '.', '--config', build_type]
        else:
            print("Invalid generator for the windows platform")
            return
    elif sys.platform.startswith('linux'):
        cmake_cmd = ['cmake', '-S', '..', '-B', build_type, '-G', 'Unix Makefiles']
        build_cmd = ['cmake', '--build', build_type]
    else:
        print("Unsupported platform")
        return

    try:
        subprocess.check_call(cmake_cmd)
        subprocess.check_call(build_cmd)
    except subprocess.CalledProcessError as e:
        print(f"Error occured: {e}")
    finally:
        os.chdir(curr_dir)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Input build type (Release/Debug)")
        exit()

    build_type = sys.argv[1]
    build_dir = 'bin/'
    generator = 'Ninja'

    cmake_build(build_dir, build_type, generator)
