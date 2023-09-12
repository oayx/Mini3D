#ifndef YAML_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define YAML_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

#include "yamlcpp/parser.h"
#include "yamlcpp/emitter.h"
#include "yamlcpp/emitterstyle.h"
#include "yamlcpp/stlemitter.h"
#include "yamlcpp/exceptions.h"

#include "yamlcpp/node/node.h"
#include "yamlcpp/node/impl.h"
#include "yamlcpp/node/convert.h"
#include "yamlcpp/node/iterator.h"
#include "yamlcpp/node/detail/impl.h"
#include "yamlcpp/node/parse.h"
#include "yamlcpp/node/emit.h"

#endif  // YAML_H_62B23520_7C8E_11DE_8A39_0800200C9A66
