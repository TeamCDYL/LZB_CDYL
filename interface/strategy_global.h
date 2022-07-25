/*******************************************************************************
 * \file        strategy_global.h
 * \authors     weiqm<4146>, linwx<4204>
 * \copyright   Copyright (c) 2021, 611.CADI All rights reserved.
 *******************************************************************************/

#ifndef STRATEGY_GLOBAL_H
#define STRATEGY_GLOBAL_H

#ifndef PI
#define PI 3.141592654
#endif

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  define STRATEGY_DECL_EXPORT __declspec(dllexport)
#  define STRATEGY_DECL_IMPORT __declspec(dllimport)
#else
#  define STRATEGY_DECL_EXPORT __attribute__((visibility("default")))
#  define STRATEGY_DECL_IMPORT __attribute__((visibility("default")))
#endif

#if defined(STRATEGY_LIBRARY)
#  define STRATEGY_EXPORT STRATEGY_DECL_EXPORT
#else
#  define STRATEGY_EXPORT STRATEGY_DECL_IMPORT
#endif

#endif // STRATEGY_GLOBAL_H
