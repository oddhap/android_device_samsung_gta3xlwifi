#!/usr/bin/env python3
"""Reject an initrd that would overlap CWA1's DTB load region."""
import struct
import sys

with open(sys.argv[1], 'rb') as image:
    header = image.read(20)
if len(header) != 20 or header[:8] != b'ANDROID!':
    raise SystemExit('Unexpected Android boot header')
size = struct.unpack_from('<I', header, 16)[0]
if size > 16777216:
    raise SystemExit(f'Ramdisk {size} bytes overlaps Samsung DTB load region')
