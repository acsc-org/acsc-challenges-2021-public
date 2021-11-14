g++ -o encoder encoder.cpp -fno-stack-protector; python3 tester.py; strip -s encoder_enc; mv encoder_enc encoder;
