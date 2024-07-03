import subprocess
import sys
import os

def cmake_build(src_dir, build_dir, build_type):
    curr_dir = os.getcwd()

    if not os.path.exists(build_dir):
        os.mkdir(build_dir)


    try:
        cmake_cmd = ['cmake', src_dir, '-B', build_dir, '-DCMAKE_C_COMPILER=clang', '-DCMAKE_CXX_COMPILER=clang++']
        subprocess.check_call(cmake_cmd)

        os.chdir(build_dir)
        build_cmd = ['cmake', '--build', '.', '--config', build_type]
        subprocess.check_call(build_cmd)

    except subprocess.CalledProcessError as e:
        print(f"Error occured: {e}")
    finally:
        os.chdir(curr_dir)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Input build type")
        exit()

    build_type = sys.argv[1]
    src_dir = '.'
    build_dir = 'bin/' + build_type

    cmake_build(src_dir, build_dir, build_type)
