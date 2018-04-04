#!/usr/bin/python

import sys
import os
import binascii
import struct

def read_entry(path, offset, size=8):
  with open(path, 'r') as f:
    f.seek(offset, 0)
    return struct.unpack('Q', f.read(size))[0]

# Read /proc/$PID/pagemap
def get_pagemap_entry(pid, addr):
  maps_path = "/proc/{0}/pagemap".format(pid)
  if not os.path.isfile(maps_path):
    print "Process {0} doesn't exist.".format(pid)
    return

  page_size = os.sysconf("SC_PAGE_SIZE")
  pagemap_entry_size = 8
  offset  = (addr / page_size) * pagemap_entry_size

  return read_entry(maps_path, offset)

def get_pfn(entry):
  return entry & 0x7FFFFFFFFFFFFF

def is_present(entry):
  return ((entry & (1 << 63)) != 0)

def is_file_page(entry):
  return ((entry & (1 << 61)) != 0)
##########################################################

# Read /proc/kpagecount
def get_pagecount(pfn):
  file_path = "/proc/kpagecount"
  offset = pfn * 8
  return read_entry(file_path, offset)

##########################################################

# Read /proc/kpageflags
def get_page_flags(pfn):
  file_path = "/proc/kpageflags"
  offset = pfn * 8
  return read_entry(file_path, offset)


if __name__ == "__main__":
  pid = sys.argv[1]
  if sys.argv[2].startswith("0x"):
    addr = long(sys.argv[2], base=16)
  else:
    addr = long(sys.argv[2])

  entry = get_pagemap_entry(pid, addr)
  pfn = get_pfn(entry)
  print "PFN: {}".format(hex(pfn))
  print "Is Present? : {}".format(is_present(entry))
  print "Is file-page: {}".format(is_file_page(entry))
  print "Page count: {}".format(get_pagecount(pfn))
  print "Page flags: {}".format(hex(get_page_flags(pfn)))
