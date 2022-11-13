import time
import sys

NULL_CHAR = 0


def say(s):
    """Prints a timestamped, self-identified message"""
    who = sys.argv[0]
    if who.endswith(".py"):
        who = who[:-3]

    s = "%s@%1.6f: %s" % (who, time.time(), s)
    print(s)


def write_to_memory(mapfile, s):
    """Writes the string s to the mapfile"""
    # say("writing %s " % s)
    
    # ts_start = time.time() * 10**9
    mapfile.seek(0)
    # ts_end = time.time() * 10**9
    # print("[QLOG] seek latency: {} ns".format(ts_end - ts_start))
    
    # I append a trailing NULL in case I'm communicating with a C program.
    s += '\0'

    # ts_start = time.time() * 10**9
    s = s.encode()
    # ts_end = time.time() * 10**9
    # print("[QLOG] encode latency: {} ns".format(ts_end - ts_start))
    
    # ts_start = time.time() * 10**9
    mapfile.write(s)
    # ts_end = time.time() * 10**9
    # print("[QLOG] write latency: {} ns".format(ts_end - ts_start))


def read_from_memory(mapfile):
    """Reads a string from the mapfile and returns that string"""
    mapfile.seek(0)
    s = []
    c = mapfile.read_byte()
    while c != NULL_CHAR:
        s.append(c)
        c = mapfile.read_byte()

    s = ''.join([chr(c) for c in s])

    say("read message length %s" % len(s))

    return s


def read_params():
    """Reads the contents of params.txt and returns them as a dict"""
    params = {}

    f = open("params.txt")

    for line in f:
        line = line.strip()
        if line:
            if line.startswith('#'):
                pass  # comment in input, ignore
            else:
                name, value = line.split('=')
                name = name.upper().strip()

                if name == "PERMISSIONS":
                    # Think octal, young man!
                    value = int(value, 8)
                elif "NAME" in name:
                    # This is a string; leave it alone.
                    pass
                else:
                    value = int(value)

                # print "name = %s, value = %d" % (name, value)

                params[name] = value

    f.close()

    return params
