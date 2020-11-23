#!/usr/bin/python3
import os
import sys
import getopt
import argparse
import subprocess


def main():
    result = ""
    sclean = os.system("make clean")
    if sclean != 0:
        print("Error!: Cannot clean")
        sys.exit(2)
    smake = os.system("make all")
    if smake != 0:
        print("Error!: Cannot compile")
        sys.exit(2)
    for file in os.listdir("../test"):
        if file.endswith(".kpl"):
            if not file.startswith("~"):
                result = subprocess.check_output(
                    f'./parser ../test/{file}', shell=True)
                resultcontent = result.decode('utf-8').split("\n")
                strf = "../test/{}".format(file.replace("example",
                                                        "result").replace("kpl", "txt"))
                f = open(strf)
                fileenc = f.read()
                filecontent = fileenc.split("\n")
                maxl = max(len(filecontent),
                           len(resultcontent))
                if fileenc.encode('utf-8') != result:
                    print(f"{strf}: False")
                    print("{:<40} {:<40} {:<10}".format(
                        "Output", "Result", "Incompatible"))
                    for i in range(maxl):
                        print("{:<40} {:<40} {:<10}".format(
                            resultcontent[i], filecontent[i], resultcontent[i] == filecontent[i]))
                else:
                    print(f"{strf}: True")
                f.close()

# def parseCmd(argv):
#     parser = argparse.ArgumentParser(description='__Check Result Compiler__.')
#     parser.add_argument('-r',help="compiler")
#     args = parser.parse_args(argv)
#     print(args)


if __name__ == '__main__':
    # parseCmd(sys.argv[1:])
    main()
