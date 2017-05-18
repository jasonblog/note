#!/bin/bash
grep -P '(^(\d{3}-){2}\d{4}$)|(^\(\d{3}\) \d{3}-\d{4}$)' file.txt
