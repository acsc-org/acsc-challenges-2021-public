#!/usr/bin/env python3
import sys
import csv

STATE_WAITING_NEWFRAME  = 0
STATE_WAITING_DATA      = 1

def read_data(csvfile):
    f = open(csvfile)
    reader = csv.DictReader(f)

    lframe_edge = 0
    lframe_prev = 1
    state = STATE_WAITING_NEWFRAME
    for row in reader:
        lframe = int(row['nLFRAME'])
        lpc = row['LPC']

        lframe_edge = lframe - lframe_prev
        lframe_prev = lframe

        if lframe_edge == 1:
            if state == STATE_WAITING_NEWFRAME:
                if lpc.startswith('START') or lpc.startswith('CYCTDIR'):
                    # Reached to the start of the new frame and decoded LPC bus is seemingly in the expected state.
                    state = STATE_WAITING_DATA
                else:
                    # Reached to the start of the new frame but decoded LPC bus is not START or CYCTDIR field.
                    # This frame is dropped wrongfully.
                    # Pad it with an unknown byte.
                    yield 'XX'
            elif state == STATE_WAITING_DATA:
                # Reached to the start of the new frame but decoded LPC bus have not seen DATA field in the previous frame.
                # The previous frame is unterminated and fused with the new frame wrongfully.
                # Pad it with two unknown bytes.
                yield 'XXXX'

                state = STATE_WAITING_NEWFRAME
            else:
                raise ValueError
        # DATA field
        elif lpc.startswith('DATA'):
            if state == STATE_WAITING_DATA:
                yield lpc.split()[1]

                state = STATE_WAITING_NEWFRAME

    f.close()

if __name__ == '__main__':
    alldata = ''.join(
        read_data(sys.argv[1])
    )
    print(alldata)
