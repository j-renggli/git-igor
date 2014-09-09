#! /usr/bin/python

"""Prepare the build directories and make the build"""

import argparse
import os
import shutil
import subprocess

class Main:
    """Main class of this tool"""
    def __init__(self):
        self.__arch = ["arm", "x86", "win"]
        self.__config = ["debug", "release"]
        self.__params = {"x86":"-DLIBS=\"/usr/lib\""
            ,"arm":"-DARCH=\"arm\" " +
                "-DCMAKE_C_COMPILER=/usr/bin/arm-linux-gnueabi-gcc " +
                "-DCMAKE_CXX_COMPILER=/usr/bin/arm-linux-gnueabi-g++ " +
                "-DLIBS=`pwd`/../../lib_arm/"
            ,"debug":"-DCMAKE_BUILD_TYPE=DEBUG"
            ,"release":"-DCMAKE_BUILD_TYPE=RELEASE"
        }
    
    def __create(self, path, arch, config):
        """Create the build folder and prepare cmake"""
        print("Creating folder " + path + " and preparing CMake")
        os.makedirs(path)
        proc = subprocess.Popen(["cmake", self.__params[config],
            self.__params[arch], "../.."], cwd=path)
        proc.wait()

    def __make(self, path):
        """Run the build"""
        proc = subprocess.Popen(["make"], cwd=path)
        proc.wait()

    def run(self):
        """Run the tool"""
        parser = argparse.ArgumentParser(
            description="Pseudo-Makefile for CMake")
        parser.add_argument("config", metavar="C", nargs="?", choices=self.__config,
            default="release", help="Target configuration")
        parser.add_argument("arch", metavar="A", nargs="?", choices=self.__arch,
            default="x86", help="Target architecture")
        parser.add_argument("--clean", action="store_const", const=True,
            default=False, help="Delete existing folder and start anew") 
        args = parser.parse_args()

        if args.arch == "x86":
            path = os.path.join("build", args.config)
        else:
            path = os.path.join("build", args.arch + "_" + args.config)

        create = False
        if os.path.exists(path):
            if args.clean:
                print("Deleting directory " + path)
                shutil.rmtree(path)
                create = True
        else:
            create = True

        if create:
            self.__create(path, args.arch, args.config)

        self.__make(path)

if __name__ == "__main__":
    main = Main()
    main.run()
