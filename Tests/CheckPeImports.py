import struct
import sys
from pathlib import Path

arch=sys.argv[1]
expected={'x86':0x14c,'x64':0x8664}[arch]
paths=[Path(p) for p in sys.argv[2:]]
if not paths:raise SystemExit('No DLL/VST3 paths supplied')
for path in paths:
 data=path.read_bytes();pe=struct.unpack_from('<I',data,60)[0]
 machine,sections=struct.unpack_from('<HH',data,pe+4)
 if machine!=expected:raise SystemExit(f'Wrong architecture: {path}')
 opt=pe+24;opt_size=struct.unpack_from('<H',data,pe+20)[0]
 magic=struct.unpack_from('<H',data,opt)[0];directory=opt+(112 if magic==0x20b else 96)
 import_rva=struct.unpack_from('<I',data,directory+8)[0]
 section_table=opt+opt_size
 def offset(rva):
  for i in range(sections):
   s=section_table+i*40;virtual_size,start,raw_size,raw=struct.unpack_from('<IIII',data,s+8)
   if start<=rva<start+max(virtual_size,raw_size):return raw+rva-start
  raise ValueError('Invalid RVA')
 def string(rva):
  start=offset(rva);return data[start:data.index(0,start)].decode('ascii')
 names=[]
 if import_rva:
  pos=offset(import_rva)
  while any(data[pos:pos+20]):
   name=struct.unpack_from('<I',data,pos+12)[0];names.append(string(name));pos+=20
 for name in names:
  if any(x in name.lower() for x in ['vcruntime','msvcp','ucrtbased','msvcr']):raise SystemExit(f'External VC runtime dependency: {path}: {name}')
  if name.lower() in ['dcomp.dll','d3d12.dll']:raise SystemExit(f'Unexpected modern GPU dependency: {path}: {name}')
 print(f'PASS {arch} {path.name}: static VC runtime; imports '+', '.join(names))
