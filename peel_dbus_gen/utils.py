# See the similarly named utility in gdbus-codegen
def camel_case_to_underscore(camel):
    s = ''
    after_lower = False
    for c in camel:
        if after_lower and c.isupper():
            s += '_'
        after_lower = c.islower()
        s += c.lower()
    return s

# See the similarly named utility in peel-gen
def escape_cpp_name(name):
    if name in ('namespace'):
        return name + '_'
    assert(name)
    if not name[0].isidentifier():
        return '_' + name
    return name
