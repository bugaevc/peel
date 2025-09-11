#pragma once

#include <peel/lang.h>
#include <peel/ZTArrayRef.h>
#include <peel/ZTUniquePtr.h>

peel_begin_header

namespace peel
{

class String;

typedef ZTArrayRef<const char * const> StrvRef;
typedef ZTUniquePtr<String[]> Strv;

} /* namespace peel */

peel_end_header
