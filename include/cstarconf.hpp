#ifndef _CSTLIB_CONFIG_HPP
#define _CSTLIB_CONFIG_HPP

#include "dset_mgmt.hpp"
#include "javalib_defs.hcst"

// inspired by bits/c++config.h
#ifdef _CSTLIB_DATASET_MANAGEMENT_HPP
#define _CSTLIB_BEGIN_DSET_SYS namespace mgr {
#define _CSTLIB_ENDING_DSET_SYS }
#endif

#ifdef _CSTLIB_JAVA_LIB_DEFS
#define _CSTLIB_BEGIN_JAVA_LIB namespace defs {
#define _CSTLIB_ENDING_JAVA_LIB }
#endif

#endif