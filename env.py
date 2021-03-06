#!/usr/bin/env python3

"""
This file is designed to perform some pre-processing on markdown prior to sending
it to pandoc. It is based on some annoying features of pandoc 2.1.3 and on the
colored block parts of FHISO preprocess-md.pl (from github.com:fhiso/website.git)
written by Richard Smith.
"""

import sys, re
import os.path

tag_open = re.compile(r'^(\s*)\{\.([a-z]+)\s*(\.{3})?\}\s*(.*)', re.DOTALL)
tag_close = re.compile(r'^(.*)\{/\}\s*$', re.DOTALL)
tag_import = re.compile(r'^\{#include\s+(.*)\}$')
tag_file = re.compile(r'^(.*)\{#file\s+(.*)\}$')

def process(fname):
    with open(fname) as f:
        tags = []
        newp = True
        for line in f:
            o,c,i = tag_open.search(line), tag_close.search(line), tag_import.search(line)
            if newp and i is not None:
                if i.group(1).endswith('.html'):
                    print('\n```{=html}')
                    with open(i.group(1)) as f2:
                        print(f2.read().strip())
                    print('```\n')
                else:
                    process(i.group(1))

            elif newp and o is not None:
                print('\n'+o.group(1)+'<div class="'+(o.group(2)+' long' if o.group(3) else o.group(2))+r'">\safeopenclass{'+o.group(2)+'}')
                if o.group(4): sys.stdout.write(o.group(4))
                if o.group(3): tags.append(o.group(2))
            elif c:
                # if c.group(1): print(c.group(1))
                print('\n'+(c.group(1) if c.group(1) else '')+'\\safecloseclass{'+tags.pop()+'}</div>\n')
            else:
                sys.stdout.write(line)
            if o is not None and not o.group(3):
                print('\n'+o.group(1)+'\\safecloseclass{'+o.group(2)+'}</div>\n')
            newp = line.isspace()


def justimport(fname):
    with open(fname) as f:
        tags = []
        newp = True
        for line in f:
            i = tag_import.search(line)
            if newp and i is not None:
                if i.group(1).endswith('.html'):
                    print('\n```{=html}')
                    with open(i.group(1)) as f2:
                        print(f2.read().strip())
                    print('```\n')
                else:
                    process(i.group(1))
            else:
                fn = tag_file.match(line)
                if fn is not None:
                    prefix,fname = fn.groups()
                    path = os.path.join('markdown',fname)
                    name = os.path.basename(fname)
                    ftype = name[name.rfind('.')+1:]
                    print(prefix+'[download `'+name+'`]('+fname+') <details class="fileview"><summary>or view `'+name+'` here:</summary>')
                    gap = ' '*len(prefix)
                    print('\n'+gap+'```'+ftype)
                    with open(path) as f2:
                        for line in f2:
                            sys.stdout.write(gap + line)
                    print(gap+'```')
                    print('\n'+gap+'</details>')
                else:
                    sys.stdout.write(line)
            newp = line.isspace()


for arg in sys.argv[1:]:
    justimport(arg)
