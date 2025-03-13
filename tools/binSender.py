import serial
import binascii
import signal
import sys
from argparse import ArgumentParser

def signal_handler(signal, frame):
    ''' Catch SIGINT/SIGTERM/SIGKILL and exit gracefully '''
    print("Stop requested...")
    sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)

def main():
    global DEBUG

    parser = ArgumentParser(
        description="Tool for sending binary payloads to a serial device. Used for testing a Delta Inverter reader, but could be used for other things."
    )
    parser.add_argument('-d', metavar='DEVICE',
                      default='/dev/ttyUSB0',
                      help='serial device port [default: /dev/ttyUSB0]'
                      )
    
    parser.add_argument('-b', metavar='BAUDRATE',
                      default=19200,
                      help='baud rate [default: 19200]')
    
    parser.add_argument('-t', metavar='TIMEOUT', type=float,
                      default=2.0,
                      help='timeout, in seconds (can be fractional, such as 1.5) [default: 2.0]')
    
    parser.add_argument('--debug', action="store_true",
                      help='show debug information')
    
    parser.add_argument('-f', metavar="PAYLOADFILE",
                        required=True,
                        help="payload file to send"
                        )
                        
    parser.add_argument('--list', action="store_true",
                        help="list serial devices"
                        )

    args = parser.parse_args()
    DEBUG = args.debug
    
    # Lets work
    
    # Get the file
    payloads = []
    with open(args.f, 'r') as f:
        lines = f.readlines()
        for line in lines:
            line = line.strip()
            if line.startswith('#') or line == "":
                # Skip empty lines or lines that start with a comment char
                continue
            
            # Try to unhexlify the string back into binary
            if DEBUG:
                print("Line: "+line)
                
            bline = binascii.unhexlify(line)
            
            if DEBUG:
                print("Payload: "+str(bline))
                
            payloads.append(bline)
    
    
    # Connect to serial
    if DEBUG:
        print("Connecting serial", args.d, args.b)
    conn = serial.Serial(args.d, args.b, timeout=args.t)
    conn.flushOutput()
    conn.flushInput()
    for payload in payloads:
        if DEBUG:
            print("S>"+str(binascii.hexlify(payload, ' ')))
        conn.write(payload)
    
    conn.close()
    
    print("Done")
    

if __name__ == "__main__":
    main()