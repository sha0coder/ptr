import os
import sys
import r2pipe

usage = '''
PTR a kernel debugger
h .............................. help.
reload ......................... reaload symbols and loaded drivers.
list ........................... list drivers.
drv [driver] ................... get the driver's address.
att [driver] ................... attach to driver.
sym [symbol] ................... find symbol of selected driver.
kw [keyword] ................... find symbol based on keyword.
all ............................ show all the driver symbols.
krn [keyword] .................. show kernel symbol based on keyword.
r [addr] [amount] .............. read bytes.
rf [addr] ...................... read function.
r2 [cmd] ....................... radare command over previous read.
dump [addr] [amount] [file] .... save blob to disk.
call [addr] [param1,param2] .... call function.
q .............................. quit.

'''

def load_kallsyms():
    kallsyms = open("/proc/kallsyms").read().strip().split('\n')
    syms = []
    for sym in kallsyms:
        if '\t' in sym:
            off = sym.find('\t')
            spl = sym[:off].strip().split()
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


def load_modules():
    modules = open('/proc/modules').read().strip().split('\n')
    mods = []
    for mod in modules:
        spl = mod.strip().split()
        addr = 0
        for s in spl:
            if '0xfff' in s:
                addr = int(s,16)
        mods.append( {'driver': spl[0], 'addr': addr} )
    return mods


def commander():
    global driver, dev, kallsyms, modules, blob

    while True:
        cmd = input('ptr> ')

        if cmd == 'q':
            break

        elif cmd == 'h':
            print(usage)

        elif cmd == 'reload':
            kallsyms = load_kallsyms()
            modules = load_modules()

        elif cmd == 'list':
            for m in modules:
                print(f'0x{m["addr"]:x} {m["driver"]}')

        elif cmd.startswith('drv '):
            drv = cmd[4:]
            for m in modules:
                if m["driver"] == drv:
                    print(f'0x{m["addr"]:x} {m["driver"]}')

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

        elif cmd.startswith('krn'):
            kw = cmd[4:]
            for sym in kallsyms:
                if kw in sym['sym']:
                    print(f'0x{sym["addr"]:x} {sym["type"]} {sym["sym"]}')
        
        elif cmd.startswith('r '):
            spl = cmd.split()
            addr = int(spl[1],16)
            sz = int(spl[2])
            os.lseek(dev, addr, os.SEEK_SET)
            blob = os.read(dev, sz)
            
        elif cmd.startswith('dump '):
            spl = cmd.split()
            addr = int(spl[1],16)
            sz = int(spl[2])
            file = spl[3]
            os.lseek(dev, addr, os.SEEK_SET)
            blob = os.read(dev, sz)
            open(file,'wb').write(blob)




def insert_driver():
    global dev
    if not os.path.isfile('ptr_drv.ko'):
        print('build the driver first, use make')
        sys.exit(1)

    os.system('insmod ptr_drv.ko')

    try:
        dev = os.open('/dev/ptr', os.O_RDONLY)
    except:
        print('driver not loader, sudo insmod ptr_drv.ko')
        sys.exit(1)


def cleanup():
    global dev
    os.close(dev)
    os.system('rmmod ptr_drv.ko')


if os.getuid() != 0:
    print('launch ptr as root.')
    sys.exit(1)

# globals
kallsyms = load_kallsyms()
modules = load_modules()
driver = None
dev = None
blob = None


insert_driver()
commander()
cleanup()

