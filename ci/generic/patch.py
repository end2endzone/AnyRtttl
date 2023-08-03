#!/usr/bin/python

import sys
import getopt
import argparse
import os
import os.path

def check_file_exists(file_path) :
   check_file = os.path.exists(file_path)
   if not check_file:
      print('File not found: ', file_path)
      sys.exit(2)

def read_file(file_path) :
   content = ''
   try:
      with open(file_path) as f:
         content=f.read()
   except IOError:
      print("Failed reading file: ", file_path)
      sys.exit(3)
   return content
   
def main(argv):
   argParser = argparse.ArgumentParser()
   argParser.add_argument("-i", "--ifile", help="Input file")
   argParser.add_argument("-o", "--ofile", help="Output file")
   argParser.add_argument("-p", "--pfile", help="Pattern file")
   argParser.add_argument("-r", "--rfile", help="Replace file")
   #argParser.add_argument("-i", "--int", type=int, help="your numeric age ")

   args = argParser.parse_args()
   #print("args=%s" % args)

   print("Patching file:  ", args.ifile)
   print("as output file: ", args.ofile)

   check_file_exists(args.ifile)
   check_file_exists(args.ofile)
   check_file_exists(args.pfile)
   check_file_exists(args.rfile)

   # Read search and replace patterns
   old_text = read_file(args.pfile)
   new_text = read_file(args.rfile)
   # print("Pattern: ", old_text)
   # print("Replace: ", new_text)

   # Read input file
   old_content = read_file(args.ifile)

   # Do the actual search and replace
   new_content = old_content.replace(old_text, new_text)

   # Write output file
   try:
      with open(args.ofile, "w") as f:
         f.write(new_content)
   except IOError:
      print("Failed writing file: ", args.ofile)
      sys.exit(3)

   if old_content == new_content:
      print("Warning! The given pattern was not found in input file.")
   else:
      print("success!")

if __name__ == "__main__":
   main(sys.argv[1:])
