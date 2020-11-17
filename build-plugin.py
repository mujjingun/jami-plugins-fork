#!/usr/bin/env python3
#
# This script must unify the Plugins build for
# every project and Operational System

import os
import sys
import platform
import argparse
import subprocess
import multiprocessing

IOS_DISTRIBUTION_NAME = "ios"
OSX_DISTRIBUTION_NAME = "osx"
ANDROID_DISTRIBUTION_NAME = "android"
WIN32_DISTRIBUTION_NAME = "win32"
UBUNTU_DISTRIBUTION_NAME = "ubuntu"

# vs vars
win_sdk_default = '10.0.16299.0'
win_toolset_default = 'v142'


def parse():
    parser = argparse.ArgumentParser(description='Builds Plugins projects')
    parser.add_argument('--projects', type=str,
                        help='Select plugins to be build.')
    parser.add_argument('--distribution')
    parser.add_argument('--processor', type=str, default="GPU",
                        help='Runtime plugin CPU/GPU setting.')

    dist = choose_distribution()

    if dist == WIN32_DISTRIBUTION_NAME:
        parser.add_argument('--toolset', default=win_toolset_default, type=str,
                            help='Windows use only, specify Visual Studio toolset version')
        parser.add_argument('--sdk', default=win_sdk_default, type=str,
                            help='Windows use only, specify Windows SDK version')

    args = parser.parse_args()
    args.projects = args.projects.split(',')
    args.processor = args.processor.split(',')

    if (args.distribution is not None):
        args.distribution = args.distribution.lower()
    else:
        args.distribution = dist

    if (len(args.processor) == 1):
        args.processor *= len(args.projects)

    validate_args(args)
    return args


def validate_args(parsed_args):
    """Validate the args values, exit if error is found"""

    # Filter unsupported distributions.
    supported_distros = [
        ANDROID_DISTRIBUTION_NAME, UBUNTU_DISTRIBUTION_NAME,
        WIN32_DISTRIBUTION_NAME
    ]

    if parsed_args.distribution not in supported_distros:
        print('Distribution \'{0}\' not supported.\nChoose one of: {1}'.format(
            parsed_args.distribution, ', '.join(supported_distros)
        ))
        sys.exit(1)

    if (len(parsed_args.processor) != len(parsed_args.projects)):
        sys.exit('Processor must be single or the same size as projects.')

    for processor in parsed_args.processor:
        if (processor not in ['GPU', 'CPU']):
            sys.exit('Processor can only be GPU or CPU.')


def choose_distribution():
    system = platform.system().lower()

    if system == "linux" or system == "linux2":
        if os.path.isfile("/etc/arch-release"):
            return "arch"
        with open("/etc/os-release") as f:
            for line in f:
                k, v = line.split("=")
                if k.strip() == 'ID':
                    return v.strip().replace('"', '').split(' ')[0]
    elif system == "darwin":
        return OSX_DISTRIBUTION_NAME
    elif system == "windows":
        return WIN32_DISTRIBUTION_NAME

    return 'Unknown'


def buildPlugin(pluginPath, processor, distribution):
    if distribution == WIN32_DISTRIBUTION_NAME:
        return subprocess.run([
            sys.executable, os.path.join(
                os.getcwd(), pluginPath + "/build-windows.py"),
            "--toolset", args.toolset,
            "--sdk", args.sdk
        ], check=True)

    environ = os.environ.copy()

    install_args = []

    if distribution == ANDROID_DISTRIBUTION_NAME:
        install_args.append('-t')
        install_args.append(ANDROID_DISTRIBUTION_NAME)
    if processor:
        install_args.append('-c')
        install_args.append(processor)
    install_args.append('-p')
    install_args.append(str(multiprocessing.cpu_count()))

    return subprocess.run([pluginPath + "/build.sh"] +
                          install_args, env=environ, check=True)


def main():
    args = parse()
    currentDir = os.getcwd()

    for i, plugin in enumerate(args.projects):
        os.chdir(currentDir + "/" + plugin)
        buildPlugin(
            currentDir + "/" + plugin,
            args.processor[i],
            args.distribution)


if __name__ == "__main__":
    main()
