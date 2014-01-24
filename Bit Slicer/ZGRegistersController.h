/*
 * Created by Mayur Pawashe on 2/7/13.
 *
 * Copyright (c) 2013 zgcoder
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the project's author nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import <Foundation/Foundation.h>
#import "ZGMemoryTypes.h"
#import "ZGVariableTypes.h"
#import <mach/thread_act.h>
#import "ZGRegister.h"

@class ZGBreakPoint;

@interface ZGRegistersController : NSResponder

typedef void(^program_counter_change_t)(void);

@property (nonatomic, assign) ZGMemoryAddress programCounter;

typedef struct
{
	char name[16];
	char value[64];
	size_t size;
	size_t offset;
	ZGRegisterType type;
} ZGRegisterEntry;

#define ZG_MAX_REGISTER_ENTRIES 128
#define ZG_REGISTER_ENTRY_IS_NULL(entry) ((entry)->name[0] == 0)

+ (int)getRegisterEntries:(ZGRegisterEntry *)entries fromGeneralPurposeThreadState:(x86_thread_state_t)threadState is64Bit:(BOOL)is64Bit;
+ (int)getRegisterEntries:(ZGRegisterEntry *)entries fromAVXThreadState:(x86_avx_state_t)avxState is64Bit:(BOOL)is64Bit;

- (void)changeProgramCounter:(ZGMemoryAddress)newProgramCounter;

- (ZGMemoryAddress)basePointer;

- (void)updateRegistersFromBreakPoint:(ZGBreakPoint *)breakPoint programCounterChange:(program_counter_change_t)programCounterChangeBlock;

- (IBAction)changeQualifier:(id)sender;

@end
