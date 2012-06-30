/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2009-2012 Zheng Te.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/** @file tweak.h
 *
 * Macro definiions.
 *
 * @author Zheng Te
 *
 */

#ifndef CYBERY_CORE_TWEAK_H
#define CYBERY_CORE_TWEAK_H


#define _BEGIN_REG_COUNTER(x) OnlyForReg##x
#define BEGIN_REG_COUNTER(x) _BEGIN_REG_COUNTER(x)

#define BEGIN_REG \
	namespace BEGIN_REG_COUNTER(__COUNTER__) \
	{	\
		static int OnlyForReg() \
		{
#define END_REG \
			return 0; \
		} \
		const int iOnlyForReg = OnlyForReg(); \
	}

#define DECLARE_HELPER()			private: struct Helper;
#define BEGIN_HELPER(class_type)	struct class_type::Helper {
#define END_HELPER(class_type) 		};

#define BEGIN_NAMESPACE(x)	namespace x {
#define END_NAMESPACE(x)	}
#define USING_NAMESPACE(x)	using namespace x;

#ifndef CYS_ASSERT
#include <assert.h>
#define CYS_ASSERT(x)	assert(x)
#endif // CYS_ASSERT

#define VTEID	uintptr_t
#define VTRID	int


#endif // CYBERY_CORE_TWEAK_H
