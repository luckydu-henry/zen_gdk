////////////////////////////////////////////////////////////////////////////////////
// NOTICE:
// Changed from OpenGL Mathmatics (glm) sources.
// Original file check https://github.com/g-truc/glm/blob/master/glm/simd/platform.h
////////////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////////
// Platform

#define FMA_PLATFORM_UNKNOWN		0x00000000
#define FMA_PLATFORM_WINDOWS		0x00010000
#define FMA_PLATFORM_LINUX			0x00020000
#define FMA_PLATFORM_APPLE			0x00040000
//#define FMA_PLATFORM_IOS			0x00080000
#define FMA_PLATFORM_ANDROID		0x00100000
#define FMA_PLATFORM_CHROME_NACL	0x00200000
#define FMA_PLATFORM_UNIX			0x00400000
#define FMA_PLATFORM_QNXNTO			0x00800000
#define FMA_PLATFORM_WINCE			0x01000000
#define FMA_PLATFORM_CYGWIN			0x02000000

#ifdef FMA_FORCE_PLATFORM_UNKNOWN
#	define FMA_PLATFORM FMA_PLATFORM_UNKNOWN
#elif defined(__CYGWIN__)
#	define FMA_PLATFORM FMA_PLATFORM_CYGWIN
#elif defined(__QNXNTO__)
#	define FMA_PLATFORM FMA_PLATFORM_QNXNTO
#elif defined(__APPLE__)
#	define FMA_PLATFORM FMA_PLATFORM_APPLE
#elif defined(WINCE)
#	define FMA_PLATFORM FMA_PLATFORM_WINCE
#elif defined(_WIN32)
#	define FMA_PLATFORM FMA_PLATFORM_WINDOWS
#elif defined(__native_client__)
#	define FMA_PLATFORM FMA_PLATFORM_CHROME_NACL
#elif defined(__ANDROID__)
#	define FMA_PLATFORM FMA_PLATFORM_ANDROID
#elif defined(__linux)
#	define FMA_PLATFORM FMA_PLATFORM_LINUX
#elif defined(__unix)
#	define FMA_PLATFORM FMA_PLATFORM_UNIX
#else
#	define FMA_PLATFORM FMA_PLATFORM_UNKNOWN
#endif//

///////////////////////////////////////////////////////////////////////////////////
// Compiler

#define FMA_COMPILER_UNKNOWN		0x00000000

// Intel
#define FMA_COMPILER_INTEL			0x00100000
#define FMA_COMPILER_INTEL14		0x00100040
#define FMA_COMPILER_INTEL15		0x00100050
#define FMA_COMPILER_INTEL16		0x00100060
#define FMA_COMPILER_INTEL17		0x00100070

// Visual C++ defines
#define FMA_COMPILER_VC				0x01000000
#define FMA_COMPILER_VC12			0x01000001
#define FMA_COMPILER_VC14			0x01000002
#define FMA_COMPILER_VC15			0x01000003
#define FMA_COMPILER_VC15_3			0x01000004
#define FMA_COMPILER_VC15_5			0x01000005
#define FMA_COMPILER_VC15_6			0x01000006
#define FMA_COMPILER_VC15_7			0x01000007
#define FMA_COMPILER_VC15_8			0x01000008
#define FMA_COMPILER_VC15_9			0x01000009
#define FMA_COMPILER_VC16			0x0100000A

// GCC defines
#define FMA_COMPILER_GCC			0x02000000
#define FMA_COMPILER_GCC46			0x020000D0
#define FMA_COMPILER_GCC47			0x020000E0
#define FMA_COMPILER_GCC48			0x020000F0
#define FMA_COMPILER_GCC49			0x02000100
#define FMA_COMPILER_GCC5			0x02000200
#define FMA_COMPILER_GCC6			0x02000300
#define FMA_COMPILER_GCC61			0x02000800
#define FMA_COMPILER_GCC7			0x02000400
#define FMA_COMPILER_GCC8			0x02000500

// CUDA
#define FMA_COMPILER_CUDA			0x10000000
#define FMA_COMPILER_CUDA75			0x10000001
#define FMA_COMPILER_CUDA80			0x10000002
#define FMA_COMPILER_CUDA90			0x10000004
#define FMA_COMPILER_CUDA_RTC	    0x10000100

// SYCL
#define FMA_COMPILER_SYCL			0x00300000

// Clang
#define FMA_COMPILER_CLANG			0x20000000
#define FMA_COMPILER_CLANG34		0x20000050
#define FMA_COMPILER_CLANG35		0x20000060
#define FMA_COMPILER_CLANG36		0x20000070
#define FMA_COMPILER_CLANG37		0x20000080
#define FMA_COMPILER_CLANG38		0x20000090
#define FMA_COMPILER_CLANG39		0x200000A0
#define FMA_COMPILER_CLANG40		0x200000B0
#define FMA_COMPILER_CLANG41		0x200000C0
#define FMA_COMPILER_CLANG42		0x200000D0

// HIP
#define FMA_COMPILER_HIP			0x40000000

// Build model
#define FMA_MODEL_32				0x00000010
#define FMA_MODEL_64				0x00000020

// Force generic C++ compiler
#ifdef FMA_FORCE_COMPILER_UNKNOWN
#	define FMA_COMPILER FMA_COMPILER_UNKNOWN

#elif defined(__INTEL_COMPILER)
#	if __INTEL_COMPILER >= 1700
#		define FMA_COMPILER FMA_COMPILER_INTEL17
#	elif __INTEL_COMPILER >= 1600
#		define FMA_COMPILER FMA_COMPILER_INTEL16
#	elif __INTEL_COMPILER >= 1500
#		define FMA_COMPILER FMA_COMPILER_INTEL15
#	elif __INTEL_COMPILER >= 1400
#		define FMA_COMPILER FMA_COMPILER_INTEL14
#	elif __INTEL_COMPILER < 1400
#		error "FMA requires ICC 2013 SP1 or newer"
#	endif

// CUDA
#elif defined(__CUDACC__)
#	if !defined(CUDA_VERSION) && !defined(FMA_FORCE_CUDA)
#		include <cuda.h>  // make sure version is defined since nvcc does not define it itself!
#	endif
#	if defined(__CUDACC_RTC__)
#		define FMA_COMPILER FMA_COMPILER_CUDA_RTC
#	elif CUDA_VERSION >= 8000
#		define FMA_COMPILER FMA_COMPILER_CUDA80
#	elif CUDA_VERSION >= 7500
#		define FMA_COMPILER FMA_COMPILER_CUDA75
#	elif CUDA_VERSION >= 7000
#		define FMA_COMPILER FMA_COMPILER_CUDA70
#	elif CUDA_VERSION < 7000
#		error "FMA requires CUDA 7.0 or higher"
#	endif

// HIP
#elif defined(__HIP__)
#	define FMA_COMPILER FMA_COMPILER_HIP

// SYCL
#elif defined(__SYCL_DEVICE_ONLY__)
#	define FMA_COMPILER FMA_COMPILER_SYCL

// Clang
#elif defined(__clang__)
#	if defined(__apple_build_version__)
#		if (__clang_major__ < 6)
#			error "FMA requires Clang 3.4 / Apple Clang 6.0 or higher"
#		elif __clang_major__ == 6 && __clang_minor__ == 0
#			define FMA_COMPILER FMA_COMPILER_CLANG35
#		elif __clang_major__ == 6 && __clang_minor__ >= 1
#			define FMA_COMPILER FMA_COMPILER_CLANG36
#		elif __clang_major__ >= 7
#			define FMA_COMPILER FMA_COMPILER_CLANG37
#		endif
#	else
#		if ((__clang_major__ == 3) && (__clang_minor__ < 4)) || (__clang_major__ < 3)
#			error "FMA requires Clang 3.4 or higher"
#		elif __clang_major__ == 3 && __clang_minor__ == 4
#			define FMA_COMPILER FMA_COMPILER_CLANG34
#		elif __clang_major__ == 3 && __clang_minor__ == 5
#			define FMA_COMPILER FMA_COMPILER_CLANG35
#		elif __clang_major__ == 3 && __clang_minor__ == 6
#			define FMA_COMPILER FMA_COMPILER_CLANG36
#		elif __clang_major__ == 3 && __clang_minor__ == 7
#			define FMA_COMPILER FMA_COMPILER_CLANG37
#		elif __clang_major__ == 3 && __clang_minor__ == 8
#			define FMA_COMPILER FMA_COMPILER_CLANG38
#		elif __clang_major__ == 3 && __clang_minor__ >= 9
#			define FMA_COMPILER FMA_COMPILER_CLANG39
#		elif __clang_major__ == 4 && __clang_minor__ == 0
#			define FMA_COMPILER FMA_COMPILER_CLANG40
#		elif __clang_major__ == 4 && __clang_minor__ == 1
#			define FMA_COMPILER FMA_COMPILER_CLANG41
#		elif __clang_major__ == 4 && __clang_minor__ >= 2
#			define FMA_COMPILER FMA_COMPILER_CLANG42
#		elif __clang_major__ >= 4
#			define FMA_COMPILER FMA_COMPILER_CLANG42
#		endif
#	endif

// Visual C++
#elif defined(_MSC_VER)
#	if _MSC_VER >= 1920
#		define FMA_COMPILER FMA_COMPILER_VC16
#	elif _MSC_VER >= 1916
#		define FMA_COMPILER FMA_COMPILER_VC15_9
#	elif _MSC_VER >= 1915
#		define FMA_COMPILER FMA_COMPILER_VC15_8
#	elif _MSC_VER >= 1914
#		define FMA_COMPILER FMA_COMPILER_VC15_7
#	elif _MSC_VER >= 1913
#		define FMA_COMPILER FMA_COMPILER_VC15_6
#	elif _MSC_VER >= 1912
#		define FMA_COMPILER FMA_COMPILER_VC15_5
#	elif _MSC_VER >= 1911
#		define FMA_COMPILER FMA_COMPILER_VC15_3
#	elif _MSC_VER >= 1910
#		define FMA_COMPILER FMA_COMPILER_VC15
#	elif _MSC_VER >= 1900
#		define FMA_COMPILER FMA_COMPILER_VC14
#	elif _MSC_VER >= 1800
#		define FMA_COMPILER FMA_COMPILER_VC12
#	elif _MSC_VER < 1800
#		error "FMA requires Visual C++ 12 - 2013 or higher"
#	endif //_MSC_VER

// G++
#elif defined(__GNUC__) || defined(__MINGW32__)
#	if __GNUC__ >= 8
#		define FMA_COMPILER FMA_COMPILER_GCC8
#	elif __GNUC__ >= 7
#		define FMA_COMPILER FMA_COMPILER_GCC7
#	elif __GNUC__ >= 6
#		define FMA_COMPILER FMA_COMPILER_GCC6
#	elif __GNUC__ >= 5
#		define FMA_COMPILER FMA_COMPILER_GCC5
#	elif __GNUC__ == 4 && __GNUC_MINOR__ >= 9
#		define FMA_COMPILER FMA_COMPILER_GCC49
#	elif __GNUC__ == 4 && __GNUC_MINOR__ >= 8
#		define FMA_COMPILER FMA_COMPILER_GCC48
#	elif __GNUC__ == 4 && __GNUC_MINOR__ >= 7
#		define FMA_COMPILER FMA_COMPILER_GCC47
#	elif __GNUC__ == 4 && __GNUC_MINOR__ >= 6
#		define FMA_COMPILER FMA_COMPILER_GCC46
#	elif ((__GNUC__ == 4) && (__GNUC_MINOR__ < 6)) || (__GNUC__ < 4)
#		error "FMA requires GCC 4.6 or higher"
#	endif

#else
#	define FMA_COMPILER FMA_COMPILER_UNKNOWN
#endif

#ifndef FMA_COMPILER
#	error "FMA_COMPILER undefined, your compiler may not be supported by FMA. Add #define FMA_COMPILER 0 to ignore this message."
#endif //FMA_COMPILER

///////////////////////////////////////////////////////////////////////////////////
// Instruction sets

// User defines: FMA_FORCE_PURE FMA_FORCE_INTRINSICS FMA_FORCE_SSE2 FMA_FORCE_SSE3 FMA_FORCE_AVX FMA_FORCE_AVX2 FMA_FORCE_AVX2

#define FMA_ARCH_MIPS_BIT	(0x10000000)
#define FMA_ARCH_PPC_BIT	(0x20000000)
#define FMA_ARCH_ARM_BIT	(0x40000000)
#define FMA_ARCH_ARMV8_BIT  (0x01000000)
#define FMA_ARCH_X86_BIT	(0x80000000)

#define FMA_ARCH_SIMD_BIT	(0x00001000)

#define FMA_ARCH_NEON_BIT	(0x00000001)
#define FMA_ARCH_SSE_BIT	(0x00000002)
#define FMA_ARCH_SSE2_BIT	(0x00000004)
#define FMA_ARCH_SSE3_BIT	(0x00000008)
#define FMA_ARCH_SSSE3_BIT	(0x00000010)
#define FMA_ARCH_SSE41_BIT	(0x00000020)
#define FMA_ARCH_SSE42_BIT	(0x00000040)
#define FMA_ARCH_AVX_BIT	(0x00000080)
#define FMA_ARCH_AVX2_BIT	(0x00000100)

#define FMA_ARCH_UNKNOWN	(0)
#define FMA_ARCH_X86		(FMA_ARCH_X86_BIT)
#define FMA_ARCH_SSE		(FMA_ARCH_SSE_BIT | FMA_ARCH_SIMD_BIT | FMA_ARCH_X86)
#define FMA_ARCH_SSE2		(FMA_ARCH_SSE2_BIT | FMA_ARCH_SSE)
#define FMA_ARCH_SSE3		(FMA_ARCH_SSE3_BIT | FMA_ARCH_SSE2)
#define FMA_ARCH_SSSE3		(FMA_ARCH_SSSE3_BIT | FMA_ARCH_SSE3)
#define FMA_ARCH_SSE41		(FMA_ARCH_SSE41_BIT | FMA_ARCH_SSSE3)
#define FMA_ARCH_SSE42		(FMA_ARCH_SSE42_BIT | FMA_ARCH_SSE41)
#define FMA_ARCH_AVX		(FMA_ARCH_AVX_BIT | FMA_ARCH_SSE42)
#define FMA_ARCH_AVX2		(FMA_ARCH_AVX2_BIT | FMA_ARCH_AVX)
#define FMA_ARCH_ARM		(FMA_ARCH_ARM_BIT)
#define FMA_ARCH_ARMV8		(FMA_ARCH_NEON_BIT | FMA_ARCH_SIMD_BIT | FMA_ARCH_ARM | FMA_ARCH_ARMV8_BIT)
#define FMA_ARCH_NEON		(FMA_ARCH_NEON_BIT | FMA_ARCH_SIMD_BIT | FMA_ARCH_ARM)
#define FMA_ARCH_MIPS		(FMA_ARCH_MIPS_BIT)
#define FMA_ARCH_PPC		(FMA_ARCH_PPC_BIT)

#if defined(FMA_FORCE_ARCH_UNKNOWN) || defined(FMA_FORCE_PURE)
#	define FMA_ARCH FMA_ARCH_UNKNOWN
#elif defined(FMA_FORCE_NEON)
#	if __ARM_ARCH >= 8
#		define FMA_ARCH (FMA_ARCH_ARMV8)
#	else
#		define FMA_ARCH (FMA_ARCH_NEON)
#	endif
#	define FMA_FORCE_INTRINSICS
#elif defined(FMA_FORCE_AVX2)
#	define FMA_ARCH (FMA_ARCH_AVX2)
#	define FMA_FORCE_INTRINSICS
#elif defined(FMA_FORCE_AVX)
#	define FMA_ARCH (FMA_ARCH_AVX)
#	define FMA_FORCE_INTRINSICS
#elif defined(FMA_FORCE_SSE42)
#	define FMA_ARCH (FMA_ARCH_SSE42)
#	define FMA_FORCE_INTRINSICS
#elif defined(FMA_FORCE_SSE41)
#	define FMA_ARCH (FMA_ARCH_SSE41)
#	define FMA_FORCE_INTRINSICS
#elif defined(FMA_FORCE_SSSE3)
#	define FMA_ARCH (FMA_ARCH_SSSE3)
#	define FMA_FORCE_INTRINSICS
#elif defined(FMA_FORCE_SSE3)
#	define FMA_ARCH (FMA_ARCH_SSE3)
#	define FMA_FORCE_INTRINSICS
#elif defined(FMA_FORCE_SSE2)
#	define FMA_ARCH (FMA_ARCH_SSE2)
#	define FMA_FORCE_INTRINSICS
#elif defined(FMA_FORCE_SSE)
#	define FMA_ARCH (FMA_ARCH_SSE)
#	define FMA_FORCE_INTRINSICS
#elif defined(FMA_FORCE_INTRINSICS) && !defined(FMA_FORCE_XYZW_ONLY)
#	if defined(__AVX2__)
#		define FMA_ARCH (FMA_ARCH_AVX2)
#	elif defined(__AVX__)
#		define FMA_ARCH (FMA_ARCH_AVX)
#	elif defined(__SSE4_2__)
#		define FMA_ARCH (FMA_ARCH_SSE42)
#	elif defined(__SSE4_1__)
#		define FMA_ARCH (FMA_ARCH_SSE41)
#	elif defined(__SSSE3__)
#		define FMA_ARCH (FMA_ARCH_SSSE3)
#	elif defined(__SSE3__)
#		define FMA_ARCH (FMA_ARCH_SSE3)
#	elif defined(__SSE2__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86_FP)
#		define FMA_ARCH (FMA_ARCH_SSE2)
#	elif defined(__i386__)
#		define FMA_ARCH (FMA_ARCH_X86)
#	elif defined(__ARM_ARCH) && (__ARM_ARCH >= 8)
#		define FMA_ARCH (FMA_ARCH_ARMV8)
#	elif defined(__ARM_NEON)
#		define FMA_ARCH (FMA_ARCH_ARM | FMA_ARCH_NEON)
#	elif defined(__arm__ ) || defined(_M_ARM)
#		define FMA_ARCH (FMA_ARCH_ARM)
#	elif defined(__mips__ )
#		define FMA_ARCH (FMA_ARCH_MIPS)
#	elif defined(__powerpc__ ) || defined(_M_PPC)
#		define FMA_ARCH (FMA_ARCH_PPC)
#	else
#		define FMA_ARCH (FMA_ARCH_UNKNOWN)
#	endif
#else
#	if defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86) || defined(__i386__)
#		define FMA_ARCH (FMA_ARCH_X86)
#	elif defined(__arm__) || defined(_M_ARM)
#		define FMA_ARCH (FMA_ARCH_ARM)
#	elif defined(__powerpc__) || defined(_M_PPC)
#		define FMA_ARCH (FMA_ARCH_PPC)
#	elif defined(__mips__)
#		define FMA_ARCH (FMA_ARCH_MIPS)
#	else
#		define FMA_ARCH (FMA_ARCH_UNKNOWN)
#	endif
#endif

#if FMA_ARCH & FMA_ARCH_AVX2
#	include <immintrin.h>
#elif FMA_ARCH & FMA_ARCH_AVX
#	include <immintrin.h>
#elif FMA_ARCH & FMA_ARCH_SSE42
#	if FMA_COMPILER & FMA_COMPILER_CLANG
#		include <popcntintrin.h>
#	endif
#	include <nmmintrin.h>
#elif FMA_ARCH & FMA_ARCH_SSE41
#	include <smmintrin.h>
#elif FMA_ARCH & FMA_ARCH_SSSE3
#	include <tmmintrin.h>
#elif FMA_ARCH & FMA_ARCH_SSE3
#	include <pmmintrin.h>
#elif FMA_ARCH & FMA_ARCH_SSE2
#	include <emmintrin.h>
#elif FMA_ARCH & FMA_ARCH_NEON
#	include <arm_neon.h>
#endif//FMA_ARCH
