# See the similarly named utility in gdbus-codegen
#
# 'XMLHttpRequest' -> 'xml_http_request'
# 'CPUQuotaPerSecUSec' -> 'cpu_quota_per_sec_usec'
# 'BindIPv6Only' -> 'bind_ipv6_only'
# 'StartupMemoryZSwapMax' -> 'startup_memory_zswap_max'
# 'ThisIsACamelCaseName' -> 'this_is_acamel_case_name' (sic)
# 'FetchDOMUsingHTTP' -> 'fetch_dom_using_http'
# 'ALLCAPS' -> 'allcaps'
def camel_case_to_underscore(camel):
    s = ''
    for i, c in enumerate(camel):
        if i > 0 and c.isupper():
            # Are we (not) entering a cluster of uppercase letters?
            prev_is_upper = camel[i - 1].isupper()
            # Are we leaving a cluster of uppercase letters?
            next_is_lower = i + 1 < len(camel) and camel[i + 1].islower()
            # Was the cluster long enough?
            prev_prev_is_upper = i >= 2 and camel[i - 2].isupper()
            if not prev_is_upper or (next_is_lower and prev_prev_is_upper):
                s += '_'
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
