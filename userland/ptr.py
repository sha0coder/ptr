import os
import sys

usage = '''
PTR a kernel debugger
h .............................. help.
att [driver] ................... attach to driver.
sym [symbol] ................... find symbol of selected driver.
kw [keyword] ................... find symbol based on keyword.
all ............................ show all the driver symbols.
krn [symbol] ................... show kernel symbols.
r [addr] [amount] .............. read bytes.
call [addr] [param1,param2] .... call function.
q .............................. quit.

'''

def load_kallsyms():
    kallsyms = open("/proc/kallsyms").read().strip().split('\n')
    syms = []
    for sym in kallsyms:
        if '\t' in sym:
            off = sym.find('\t')
            spl = sym[:off].strip().split(' ')
            entry = {
                    'addr': int(spl[0],16),
                    'type': spl[1],
                    'sym': spl[2],
                    'driver': sym[off+1:][1:-1]
            } 

        else:
            spl = sym.strip().split(' ')
            entry = {
                    'addr': int(spl[0],16),
                    'type': spl[1],
                    'sym': spl[2],
                    'driver': ''
            } 
        syms.append(entry) 

    return syms


if os.getuid() != 0:
    print('launch ptr as root.')
    sys.exit(1)

kallsyms = load_kallsyms()
driver = None

try:
    dev = os.open('/dev/ptr', os.O_RDONLY)
except:
    print('driver not loader, sudo insmod ptr_drv.ko')
    sys.exit(1)


while True:
    cmd = input('ptr> ')

    if cmd == 'q':
        break

    elif cmd == 'h':
        print(usage)

    elif cmd.startswith('att '):
        driver = cmd[4:]
        print(f'attached to {driver} driver.')

    elif cmd == 'all':
        if not driver:
            print('attach to driver first')
            continue
        for sym in kallsyms:
            if sym['driver'] == driver:
                print(f'0x{sym["addr"]:x} {sym["type"]} {sym["sym"]}')

    elif cmd.startswith('sym'):
        lookup = cmd[4:]
        if not driver:
            print('attach to driver first')
            continue
        for sym in kallsyms:
            if sym['driver'] == driver and sym['sym'] == lookup:
                print(f'0x{sym["addr"]:x} {sym["type"]} {sym["sym"]}')

    elif cmd.startswith('kw'):
        lookup = cmd[3:]
        if not driver:
            print('attach to driver first')
            continue
        for sym in kallsyms:
            if sym['driver'] == driver and lookup in sym['sym']:
                print(f'0x{sym["addr"]:x} {sym["type"]} {sym["sym"]}')





os.close(dev)
