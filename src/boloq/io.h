#pragma once
#include <boloq/details/visitors/io.h>

namespace std {

std::ostream& operator<<(std::ostream& os, const boloq::boolean_function& cmb) {
    boloq::io_visitor<boloq::boolean_function> v(os);
    cmb.accept(v);
    return os;
}

std::ostream& operator<<(std::ostream& os, const boloq::combination& cmb) {
    boloq::io_visitor<boloq::combination> v(os);
    cmb.accept(v);
    return os;
}

}

