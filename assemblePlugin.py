import os
import argparse
import sys
import subprocess
from zipfile import ZipFile
import platform

def parser():
    parser = argparse.ArgumentParser(description='Build some plugins.')
    parser.add_argument('--plugins', type=str,
                    help='Names of plugins to be build')

    args = parser.parse_args()
    return args

def main():
    args = parser()
    
    system = platform.system().lower()
    if system == "linux" or system == "linux2":
        osBuildPath = "/build-local"
    elif system == "darwin":
        sys.exit("Plugins not supported on MacOS and IOS")
    elif system == "windows":
        osBuildPath = "/msvc"

    plugins = args.plugins.split(',')
    for plugin in plugins:
        root = os.path.dirname(os.path.abspath(__file__))
        outputJPL = root + "/build/" + plugin + "/" + plugin + ".jpl"
        outputBuild = root + "/"+ plugin + osBuildPath + "/jpl"
        
        with ZipFile(outputJPL, 'w') as zipObj:
            for folderName, subfolders, filenames in os.walk(outputBuild):
                for filename in filenames:
                    filePath = os.path.join(folderName, filename)
                    zipObj.write(filePath, folderName.split("/")[-1][4:]+"/"+filename)
        


if __name__=='__main__':
    main()