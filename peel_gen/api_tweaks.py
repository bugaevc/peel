from peel_gen.exceptions import UnsupportedForNowException

# tweak_ident -> [(key, ...)]
tweaks = dict()

def lookup(tweak_ident, kind=None):
    if tweak_ident not in tweaks:
        return
    for tweak in tweaks[tweak_ident]:
        if kind is not None and tweak[0] != kind:
            continue
        yield tweak

def load_from_file(path):
    with open(path) as tweaks_f:
        for line in tweaks_f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            parts = line.split()
            tweak_ident = parts[1]
            if tweak_ident not in tweaks:
                tweaks[tweak_ident] = []
            tweaks[tweak_ident].append((parts[0], *parts[2:]))

def skip_if_needed(tweak_ident):
    for tweak in lookup(tweak_ident, 'skip'):
        raise UnsupportedForNowException('explicitly skipped')

def ifdef_if_needed(tweak_ident):
    for tweak in lookup(tweak_ident, 'ifdef'):
        return '#ifdef ' + tweak[1]

def endif_if_needed(tweak_ident):
    for tweak in lookup(tweak_ident, 'ifdef'):
        return '#endif /* {} */'.format(tweak[1])

def ifdef_for_non_opaque(tweak_ident):
    for tweak in lookup(tweak_ident, 'opaque-ifndef'):
        return '#ifdef ' + tweak[1]

def endif_for_non_opaque(tweak_ident):
    for tweak in lookup(tweak_ident, 'opaque-ifndef'):
        return '#endif /* {} */'.format(tweak[1])
