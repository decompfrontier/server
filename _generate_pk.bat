@echo off
cd packet-generator\
python packet-generator.py -l c++ -s * -o ../gimuserver/packets/
cd ..\
