#!/usr/bin/env python
import sys
import fileinput
import subprocess
import re
import argparse

def readTestCases(filename):
    testCases=[]
    testCaseNo=0
    testCasePart=0
    for line in fileinput.input(filename):
        line = line.strip()
        if (line and not line.startswith("#")):
            if  testCasePart==0:
                testCases.append({'args':line})
                testCasePart+=1
            elif testCasePart==1:
                testCases[testCaseNo].update({'input':line})
                testCasePart+=1
            elif testCasePart==2:
                testCases[testCaseNo].update({'output':line})
                testCasePart+=1
            elif testCasePart==3:
                testCases[testCaseNo].update({'md5sum':line})
                testCasePart=0
                testCaseNo+=1

    return testCases


executable="../../Build/VS/Win32/Debug/NaCZIrCmd.exe"
sourcefolder="D:/PICTURES/NaCZIrTestData/"
destinationfolder="D:/TFSJBL/NaCZIr/Test/Out/"
testdatafile="./testdata_win.txt"
parser = argparse.ArgumentParser()	
parser.add_argument("-k","--skip",help="skip the specified number of test-cases",default=0)
parser.add_argument("-e","--executable",help="filename of the 'NaCZIrCmd' executable")	
parser.add_argument("-s","--sourcefolder",help="foldername where the sample data can be found")	
parser.add_argument("-d","--destinationfolder",help="foldername where the results will be put")	
parser.add_argument("-t","--testdatafile",help="filename for the testdata")	
args = parser.parse_args()
if args.executable:
#	print("Executable specified:" + args.executable)
    executable = args.executable
if args.sourcefolder:
#	print("Sourcefolder specified:" + args.sourcefolder)
    sourcefolder = args.sourcefolder
if args.destinationfolder:
#	print("Destinationfolder specified:" + args.destinationfolder)
    destinationfolder = args.destinationfolder
if args.testdatafile:
    testdatafile = args.testdatafile

testCases = readTestCases(testdatafile)

testCasesToSkip=0
if args.skip:
    testCasesToSkip = int(args.skip)	


no = 0
cntOk=0
cntFailure=0
for testCase in testCases:
    if testCasesToSkip > 0:
        testCasesToSkip-=1
        print(str(no) + " -> SKIPPED")
    else:        
        cmdlineargs = []
        cmdlineargs.append(executable)
        cmdlineargs.extend(testCase['args'].split(' '))
        cmdlineargs.append("--source")
        cmdlineargs.append(sourcefolder+testCase['input'])
        cmdlineargs.append("--output")
        cmdlineargs.append(destinationfolder+testCase['output'])
        cmdlineargs.append("--calc-hash")
#        cmdlineargs.append("--drawtileboundaries")
#        print(cmdlineargs)
        p = subprocess.Popen(cmdlineargs,stdout=subprocess.PIPE, universal_newlines=True)
        (md5sumoutput,err)=p.communicate()
#        print("OUTPUT"+md5sumoutput)
        reResult = re.search("^hash of result: ([0-9a-fA-F]{32})",md5sumoutput,re.MULTILINE)
        chksum = reResult.group(1)
#    print( "chksum:"+chksum+"   testCase:"+testCase['md5sum'])
        if (chksum.upper() == testCase['md5sum'].upper()):
            print(str(no) + " -> OK")
            cntOk+=1
        else:
            print(str(no) + "-> FAILURE")
            print(str(no) + "-> FAILURE (is:"+chksum.upper()+" expected:"+testCase['md5sum'].upper()+")")
            cntFailure+=1
    no+=1

print("")
print("Testcases: "+str(cntOk+cntFailure)+"  OK: "+str(cntOk)+"  FAILURE: "+str(cntFailure))

