/*
 *  types.h
 *  mse
 *
 *  Created by juweihua on 4/4/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifdef _MACOS
typedef int BOOL;
typedef void* HMODULE;
#endif

#ifdef _64
typedef long INT ;
#else
typedef int INT ;
#endif