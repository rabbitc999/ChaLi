# -*- coding: utf-8 -*-
"""逐章试编译：给每章套一个最小壳文档，定位转换错误。"""
import glob
import io
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
LATEX = os.path.normpath(os.path.join(HERE, '..'))
sys.path.insert(0, os.path.normpath(os.path.join(LATEX, '../../../tools')))
from project_temp import tempfile
TMP = tempfile.mkdtemp(prefix='book-chapters-')
os.chdir(LATEX)

wrapper = r"""\documentclass[cn,ctexfont,fancy,newtx]{colorful}
\input{%s/preamble}
\begin{document}
\input{%s}
\end{document}
"""

fail = []
for tex in sorted(glob.glob(os.path.join(LATEX, 'chapters', '*.tex'))):
    name = os.path.splitext(os.path.basename(tex))[0]
    source = os.path.join(TMP, name + '_t.tex')
    io.open(source, 'w', encoding='utf-8').write(
        wrapper % (LATEX.replace('\\', '/'), tex.replace('\\', '/')))
    for f in glob.glob(os.path.join(TMP, name + '_t.*')):
        if f.endswith(('.aux', '.log', '.pdf')):
            os.remove(f)
    r = subprocess.run(['xelatex', '-interaction=nonstopmode', '-output-directory=' + TMP, source],
                       capture_output=True, timeout=240)
    log = io.open(os.path.join(TMP, name + '_t.log'), encoding='utf-8', errors='replace').read()
    errs = re.findall(r'^!.*$', log, re.M)
    status = 'ok' if not errs else 'FAIL(%d)' % len(errs)
    if errs:
        fail.append(name)
    print('%-14s %s %s' % (name, status, errs[:2]))

print('FAILS:', fail)
