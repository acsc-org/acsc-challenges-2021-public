#!/bin/bash
exec 2>/dev/null
cd /home/carot/
timeout 600 /home/carot/proxy.py
