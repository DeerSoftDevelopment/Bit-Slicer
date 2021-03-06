/*
 * Copyright (c) 2014 Mayur Pawashe
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

#include "ZGThreadStates.h"

#if TARGET_CPU_ARM64
typedef arm_neon_state_t zg_float_state_t;
#else
typedef x86_float_state_t zg_float_state_t;
#endif

bool ZGGetGeneralThreadState(zg_thread_state_t *threadState, thread_act_t thread, mach_msg_type_number_t *stateCount)
{
#if TARGET_CPU_ARM64
	(void)threadState;
	(void)thread;
	(void)stateCount;
	return false;
#else
	mach_msg_type_number_t localStateCount = x86_THREAD_STATE_COUNT;
	bool success = (thread_get_state(thread, x86_THREAD_STATE, (thread_state_t)threadState, &localStateCount) == KERN_SUCCESS);
	if (stateCount != NULL) *stateCount = localStateCount;
	return success;
#endif
}

bool ZGSetGeneralThreadState(zg_thread_state_t *threadState, thread_act_t thread, mach_msg_type_number_t stateCount)
{
#if TARGET_CPU_ARM64
	(void)threadState;
	(void)thread;
	(void)stateCount;
	return false;
#else
	return (thread_set_state(thread, x86_THREAD_STATE, (thread_state_t)threadState, stateCount) == KERN_SUCCESS);
#endif
}

ZGMemoryAddress ZGInstructionPointerFromGeneralThreadState(zg_thread_state_t *threadState, ZGProcessType type)
{
#if TARGET_CPU_ARM64
	(void)threadState;
	(void)type;
	return 0;
#else
	ZGMemoryAddress instructionPointer = (ZG_PROCESS_TYPE_IS_X86_64(type)) ? threadState->uts.ts64.__rip : threadState->uts.ts32.__eip;
	return instructionPointer;
#endif
}

ZGMemoryAddress ZGBasePointerFromGeneralThreadState(zg_thread_state_t *threadState, ZGProcessType type)
{
#if TARGET_CPU_ARM64
	(void)threadState;
	(void)type;
	return 0;
#else
	ZGMemoryAddress basePointer = (ZG_PROCESS_TYPE_IS_X86_64(type)) ? threadState->uts.ts64.__rbp : threadState->uts.ts32.__ebp;
	return basePointer;
#endif
}

bool ZGGetDebugThreadState(zg_debug_state_t *debugState, thread_act_t thread, mach_msg_type_number_t *stateCount)
{
#if TARGET_CPU_ARM64
	(void)debugState;
	(void)thread;
	(void)stateCount;
	return false;
#else
	mach_msg_type_number_t localStateCount = x86_DEBUG_STATE_COUNT;
	bool success = (thread_get_state(thread, x86_DEBUG_STATE, (thread_state_t)debugState, &localStateCount) == KERN_SUCCESS);
	if (stateCount != NULL) *stateCount = localStateCount;
	return success;
#endif
}

bool ZGSetDebugThreadState(zg_debug_state_t *debugState, thread_act_t thread, mach_msg_type_number_t stateCount)
{
#if TARGET_CPU_ARM64
	(void)debugState;
	(void)thread;
	(void)stateCount;
	return false;
#else
	return (thread_set_state(thread, x86_DEBUG_STATE, (thread_state_t)debugState, stateCount) == KERN_SUCCESS);
#endif
}

#if TARGET_CPU_ARM64
#else
// For some reason for AVX set/get thread functions, it is important to distinguish between 32 vs 64 bit,
// even when I can use more generic versions for general purpose and debug registers

static bool ZGGetAVXThreadState(zg_vector_state_t * avxState, thread_act_t thread, mach_msg_type_number_t *stateCount, ZGProcessType type)
{
	bool isX86_64 = ZG_PROCESS_TYPE_IS_X86_64(type);
	mach_msg_type_number_t localStateCount = isX86_64 ? x86_AVX_STATE64_COUNT : x86_AVX_STATE32_COUNT;
	bool success = (thread_get_state(thread, isX86_64 ? x86_AVX_STATE64 : x86_AVX_STATE32, isX86_64 ? (thread_state_t)&(avxState->ufs.as64) : (thread_state_t)&(avxState->ufs.as32), &localStateCount) == KERN_SUCCESS);
	
	if (stateCount != NULL) *stateCount = localStateCount;
	
	return success;
}

static bool ZGSetAVXThreadState(zg_vector_state_t *avxState, thread_act_t thread, mach_msg_type_number_t stateCount, ZGProcessType type)
{
	bool isX86_64 = ZG_PROCESS_TYPE_IS_X86_64(type);
	return (thread_set_state(thread, isX86_64 ? x86_AVX_STATE64 : x86_AVX_STATE32, isX86_64 ? (thread_state_t)&(avxState->ufs.as64) : (thread_state_t)&(avxState->ufs.as32), stateCount) == KERN_SUCCESS);
}

// I will assume I have to provide 64-bit flag for same reasons I have to for AVX (see above)

static bool ZGGetFloatThreadState(zg_float_state_t *floatState, thread_act_t thread, mach_msg_type_number_t *stateCount, ZGProcessType type)
{
	bool isX86_64 = ZG_PROCESS_TYPE_IS_X86_64(type);
	mach_msg_type_number_t localStateCount = isX86_64 ? x86_FLOAT_STATE64_COUNT : x86_FLOAT_STATE32_COUNT;
	bool success = (thread_get_state(thread, isX86_64 ? x86_FLOAT_STATE64 : x86_FLOAT_STATE32, isX86_64 ? (thread_state_t)&(floatState->ufs.fs64) : (thread_state_t)&(floatState->ufs.fs32), &localStateCount) == KERN_SUCCESS);
	
	if (stateCount != NULL) *stateCount = localStateCount;
	
	return success;
}

static bool ZGSetFloatThreadState(zg_float_state_t *floatState, thread_act_t thread, mach_msg_type_number_t stateCount, ZGProcessType type)
{
	bool isX86_64 = ZG_PROCESS_TYPE_IS_X86_64(type);
	return (thread_set_state(thread, isX86_64 ? x86_FLOAT_STATE64 : x86_FLOAT_STATE32, isX86_64 ? (thread_state_t)&floatState->ufs.fs64 : (thread_state_t)&floatState->ufs.fs32, stateCount) == KERN_SUCCESS);
}

#endif

bool ZGGetVectorThreadState(zg_vector_state_t *vectorState, thread_act_t thread, mach_msg_type_number_t *stateCount, ZGProcessType type, bool *hasAVXSupport)
{
#if TARGET_CPU_ARM64
	(void)vectorState;
	(void)thread;
	(void)stateCount;
	(void)type;
	(void)hasAVXSupport;
	return false;
#else
	if (ZGGetAVXThreadState((zg_vector_state_t *)vectorState, thread, stateCount, type))
	{
		if (hasAVXSupport != NULL) *hasAVXSupport = true;
		return true;
	}
	
	if (hasAVXSupport != NULL) *hasAVXSupport = false;
	
	return ZGGetFloatThreadState((zg_float_state_t *)vectorState, thread, stateCount, type);
#endif
}

bool ZGSetVectorThreadState(zg_vector_state_t *vectorState, thread_act_t thread, mach_msg_type_number_t stateCount, ZGProcessType type)
{
#if TARGET_CPU_ARM64
	(void)vectorState;
	(void)thread;
	(void)stateCount;
	(void)type;
	return false;
#else
	if (ZGSetAVXThreadState((zg_vector_state_t *)vectorState, thread, stateCount, type))
	{
		return true;
	}
	
	return ZGSetFloatThreadState((zg_float_state_t *)vectorState, thread, stateCount, type);
#endif
}


