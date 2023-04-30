#!/bin/python3

import os
import sys
import re

def strip_non_alpha(s):
    return ''.join(c for c in s if c.isalpha())

def pad_with_zeros(s):
    arr = s.encode('utf-8')
    arr = arr.ljust(64, b'\0')
    return arr.decode('utf-8')

if len(sys.argv) < 2:
    print("No input file provided")
    exit(1)

if len(sys.argv) > 2:
    print("Too many arguments")
    exit(1)

filename = sys.argv[1]
if not os.path.exists(filename):
    print(f"File '{filename}' not found")
    exit(1)

text = ""
with open(filename, "r") as input_file:
    text = input_file.read()

words = (strip_non_alpha(word) for word in text.split())
words = (word for word in words if len(word) > 0)
words = (word for word in words if re.fullmatch(r"[ILVX]+", word) is None)
words = (word.lower() for word in words)
words = (''.join(pad_with_zeros(word)) for word in words)
text = ''.join(words)

with open(filename + ".data", "w+") as output_file:
    output_file.write(text);
