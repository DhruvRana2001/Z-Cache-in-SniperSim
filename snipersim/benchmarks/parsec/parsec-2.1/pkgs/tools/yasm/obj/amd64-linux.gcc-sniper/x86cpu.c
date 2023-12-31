/* ANSI-C code produced by genperf */
/* Command-line: genperf /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf x86cpu.c */
#line 26 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"

#include <util.h>
RCSID("$Id: x86cpu.gperf 2092 2008-05-14 03:46:41Z peter $");

#include <ctype.h>
#include <libyasm.h>
#include <libyasm/phash.h>

#include "modules/arch/x86/x86arch.h"

#define PROC_8086	0
#define PROC_186	1
#define PROC_286	2
#define PROC_386	3
#define PROC_486	4
#define PROC_586	5
#define PROC_686	6
#define PROC_p2		7
#define PROC_p3		8
#define PROC_p4		9
#define PROC_prescott	10
#define PROC_conroe	11
#define PROC_penryn	12
#define PROC_nehalem	13
#define PROC_westmere   14
#define PROC_sandybridge 15

static void
x86_cpu_intel(wordptr cpu, unsigned int data)
{
    BitVector_Empty(cpu);

    BitVector_Bit_On(cpu, CPU_Priv);
    if (data >= PROC_286)
        BitVector_Bit_On(cpu, CPU_Prot);
    if (data >= PROC_386)
        BitVector_Bit_On(cpu, CPU_SMM);
    if (data >= PROC_sandybridge)
        BitVector_Bit_On(cpu, CPU_AVX);
    if (data >= PROC_westmere) {
        BitVector_Bit_On(cpu, CPU_AES);
        BitVector_Bit_On(cpu, CPU_CLMUL);
    }
    if (data >= PROC_nehalem) {
        BitVector_Bit_On(cpu, CPU_SSE42);
        BitVector_Bit_On(cpu, CPU_XSAVE);
    }
    if (data >= PROC_penryn)
        BitVector_Bit_On(cpu, CPU_SSE41);
    if (data >= PROC_conroe)
        BitVector_Bit_On(cpu, CPU_SSSE3);
    if (data >= PROC_prescott)
        BitVector_Bit_On(cpu, CPU_SSE3);
    if (data >= PROC_p4)
        BitVector_Bit_On(cpu, CPU_SSE2);
    if (data >= PROC_p3)
        BitVector_Bit_On(cpu, CPU_SSE);
    if (data >= PROC_p2)
        BitVector_Bit_On(cpu, CPU_MMX);
    if (data >= PROC_486)
        BitVector_Bit_On(cpu, CPU_FPU);
    if (data >= PROC_prescott)
        BitVector_Bit_On(cpu, CPU_EM64T);

    if (data >= PROC_p4)
        BitVector_Bit_On(cpu, CPU_P4);
    if (data >= PROC_p3)
        BitVector_Bit_On(cpu, CPU_P3);
    if (data >= PROC_686)
        BitVector_Bit_On(cpu, CPU_686);
    if (data >= PROC_586)
        BitVector_Bit_On(cpu, CPU_586);
    if (data >= PROC_486)
        BitVector_Bit_On(cpu, CPU_486);
    if (data >= PROC_386)
        BitVector_Bit_On(cpu, CPU_386);
    if (data >= PROC_286)
        BitVector_Bit_On(cpu, CPU_286);
    if (data >= PROC_186)
        BitVector_Bit_On(cpu, CPU_186);
    BitVector_Bit_On(cpu, CPU_086);
}

static void
x86_cpu_ia64(wordptr cpu, unsigned int data)
{
    BitVector_Empty(cpu);
    BitVector_Bit_On(cpu, CPU_Priv);
    BitVector_Bit_On(cpu, CPU_Prot);
    BitVector_Bit_On(cpu, CPU_SMM);
    BitVector_Bit_On(cpu, CPU_SSE2);
    BitVector_Bit_On(cpu, CPU_SSE);
    BitVector_Bit_On(cpu, CPU_MMX);
    BitVector_Bit_On(cpu, CPU_FPU);
    BitVector_Bit_On(cpu, CPU_IA64);
    BitVector_Bit_On(cpu, CPU_P4);
    BitVector_Bit_On(cpu, CPU_P3);
    BitVector_Bit_On(cpu, CPU_686);
    BitVector_Bit_On(cpu, CPU_586);
    BitVector_Bit_On(cpu, CPU_486);
    BitVector_Bit_On(cpu, CPU_386);
    BitVector_Bit_On(cpu, CPU_286);
    BitVector_Bit_On(cpu, CPU_186);
    BitVector_Bit_On(cpu, CPU_086);
}

#define PROC_bulldozer	11
#define PROC_k10    10
#define PROC_venice 9
#define PROC_hammer 8
#define PROC_k7     7
#define PROC_k6     6

static void
x86_cpu_amd(wordptr cpu, unsigned int data)
{
    BitVector_Empty(cpu);

    BitVector_Bit_On(cpu, CPU_Priv);
    BitVector_Bit_On(cpu, CPU_Prot);
    BitVector_Bit_On(cpu, CPU_SMM);
    BitVector_Bit_On(cpu, CPU_3DNow);
    if (data >= PROC_bulldozer)
        BitVector_Bit_On(cpu, CPU_SSE5);
    if (data >= PROC_k10)
        BitVector_Bit_On(cpu, CPU_SSE4a);
    if (data >= PROC_venice)
        BitVector_Bit_On(cpu, CPU_SSE3);
    if (data >= PROC_hammer)
        BitVector_Bit_On(cpu, CPU_SSE2);
    if (data >= PROC_k7)
        BitVector_Bit_On(cpu, CPU_SSE);
    if (data >= PROC_k6)
        BitVector_Bit_On(cpu, CPU_MMX);
    BitVector_Bit_On(cpu, CPU_FPU);

    if (data >= PROC_hammer)
        BitVector_Bit_On(cpu, CPU_Hammer);
    if (data >= PROC_k7)
        BitVector_Bit_On(cpu, CPU_Athlon);
    if (data >= PROC_k6)
        BitVector_Bit_On(cpu, CPU_K6);
    BitVector_Bit_On(cpu, CPU_686);
    BitVector_Bit_On(cpu, CPU_586);
    BitVector_Bit_On(cpu, CPU_486);
    BitVector_Bit_On(cpu, CPU_386);
    BitVector_Bit_On(cpu, CPU_286);
    BitVector_Bit_On(cpu, CPU_186);
    BitVector_Bit_On(cpu, CPU_086);
}

static void
x86_cpu_set(wordptr cpu, unsigned int data)
{
    BitVector_Bit_On(cpu, data);
}

static void
x86_cpu_clear(wordptr cpu, unsigned int data)
{
    BitVector_Bit_Off(cpu, data);
}

static void
x86_cpu_set_sse4(wordptr cpu, unsigned int data)
{
    BitVector_Bit_On(cpu, CPU_SSE41);
    BitVector_Bit_On(cpu, CPU_SSE42);
}

static void
x86_cpu_clear_sse4(wordptr cpu, unsigned int data)
{
    BitVector_Bit_Off(cpu, CPU_SSE41);
    BitVector_Bit_Off(cpu, CPU_SSE42);
}

#line 212 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
struct cpu_parse_data {
    const char *name;
    void (*handler) (wordptr cpu, unsigned int data);
    unsigned int data;
};
static const struct cpu_parse_data *
cpu_find(const char *key, size_t len)
{
  static const struct cpu_parse_data pd[129] = {
#line 343 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"avx",		x86_cpu_set,	CPU_AVX},
#line 238 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"ppro",		x86_cpu_intel,	PROC_686},
#line 329 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"sse41",		x86_cpu_set,	CPU_SSE41},
#line 226 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"80386",		x86_cpu_intel,	PROC_386},
#line 255 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentium-iv",	x86_cpu_intel,	PROC_p4},
#line 222 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"286",		x86_cpu_intel,	PROC_286},
#line 253 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentium-4",	x86_cpu_intel,	PROC_p4},
#line 295 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"3dnow",		x86_cpu_set,	CPU_3DNow},
#line 246 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentium3",	x86_cpu_intel,	PROC_p3},
#line 337 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"sse4",		x86_cpu_set_sse4,	0},
#line 333 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"sse42",		x86_cpu_set,	CPU_SSE42},
#line 323 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"em64t",		x86_cpu_set,	CPU_EM64T},
#line 351 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pclmulqdq",	x86_cpu_set,	CPU_CLMUL},
#line 227 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"i386",		x86_cpu_intel,	PROC_386},
#line 330 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nosse41",	x86_cpu_clear,	CPU_SSE41},
#line 317 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"privileged",	x86_cpu_set,	CPU_Priv},
#line 307 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"undoc",		x86_cpu_set,	CPU_Undoc},
#line 250 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"katmai",		x86_cpu_intel,	PROC_p3},
#line 346 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nofma",		x86_cpu_clear,	CPU_FMA},
#line 287 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"sse",		x86_cpu_set,	CPU_SSE},
#line 265 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"clawhammer",	x86_cpu_amd,	PROC_hammer},
#line 273 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"conroe",		x86_cpu_intel,	PROC_conroe},
#line 249 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentium-iii",	x86_cpu_intel,	PROC_p3},
#line 350 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"noclmul",	x86_cpu_clear,	CPU_CLMUL},
#line 296 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"no3dnow",	x86_cpu_clear,	CPU_3DNow},
#line 300 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"noamd",		x86_cpu_clear,	CPU_AMD},
#line 220 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"80186",		x86_cpu_intel,	PROC_186},
#line 252 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentium4",	x86_cpu_intel,	PROC_p4},
#line 334 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nosse42",	x86_cpu_clear,	CPU_SSE42},
#line 288 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nosse",		x86_cpu_clear,	CPU_SSE},
#line 243 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentiumii",	x86_cpu_intel,	PROC_p2},
#line 223 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"80286",		x86_cpu_intel,	PROC_286},
#line 224 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"i286",		x86_cpu_intel,	PROC_286},
#line 291 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"sse3",		x86_cpu_set,	CPU_SSE3},
#line 321 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"padlock",	x86_cpu_set,	CPU_PadLock},
#line 306 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"noprotected",	x86_cpu_clear,	CPU_Prot},
#line 352 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nopclmulqdq",	x86_cpu_clear,	CPU_CLMUL},
#line 272 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"prescott",	x86_cpu_intel,	PROC_prescott},
#line 274 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"core2",		x86_cpu_intel,	PROC_conroe},
#line 266 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"opteron",	x86_cpu_amd,	PROC_hammer},
#line 277 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"westmere",	x86_cpu_intel,	PROC_westmere},
#line 314 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"noobsolete",	x86_cpu_clear,	CPU_Obs},
#line 319 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"svm",		x86_cpu_set,	CPU_SVM},
#line 345 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"fma",		x86_cpu_set,	CPU_FMA},
#line 301 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"smm",		x86_cpu_set,	CPU_SMM},
#line 303 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"prot",		x86_cpu_set,	CPU_Prot},
#line 311 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"obs",		x86_cpu_set,	CPU_Obs},
#line 234 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"p5",		x86_cpu_intel,	PROC_586},
#line 335 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"sse4a",		x86_cpu_set,	CPU_SSE4a},
#line 219 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"186",		x86_cpu_intel,	PROC_186},
#line 348 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"noaes",		x86_cpu_clear,	CPU_AES},
#line 263 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"k8",		x86_cpu_amd,	PROC_hammer},
#line 225 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"386",		x86_cpu_intel,	PROC_386},
#line 244 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentium-ii",	x86_cpu_intel,	PROC_p2},
#line 269 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"venice",		x86_cpu_amd,	PROC_venice},
#line 278 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"sandybridge",	x86_cpu_intel,	PROC_sandybridge},
#line 270 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"k10",		x86_cpu_amd,	PROC_k10},
#line 290 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nosse2",		x86_cpu_clear,	CPU_SSE2},
#line 318 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"noprivileged",	x86_cpu_clear,	CPU_Priv},
#line 332 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nosse4.2",	x86_cpu_clear,	CPU_SSE42},
#line 229 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"80486",		x86_cpu_intel,	PROC_486},
#line 232 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"i586",		x86_cpu_intel,	PROC_586},
#line 286 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nommx",		x86_cpu_clear,	CPU_MMX},
#line 315 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"priv",		x86_cpu_set,	CPU_Priv},
#line 248 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentiumiii",	x86_cpu_intel,	PROC_p3},
#line 241 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentium2",	x86_cpu_intel,	PROC_p2},
#line 257 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"ia64",		x86_cpu_ia64,	0},
#line 264 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"hammer",		x86_cpu_amd,	PROC_hammer},
#line 245 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"p3",		x86_cpu_intel,	PROC_p3},
#line 239 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentiumpro",	x86_cpu_intel,	PROC_686},
#line 308 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"noundoc",	x86_cpu_clear,	CPU_Undoc},
#line 218 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"8086",		x86_cpu_intel,	PROC_8086},
#line 327 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"sse4.1",		x86_cpu_set,	CPU_SSE41},
#line 316 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nopriv",		x86_cpu_clear,	CPU_Priv},
#line 328 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nosse4.1",	x86_cpu_clear,	CPU_SSE41},
#line 324 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"noem64t",	x86_cpu_clear,	CPU_EM64T},
#line 275 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"penryn",		x86_cpu_intel,	PROC_penryn},
#line 309 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"undocumented",	x86_cpu_set,	CPU_Undoc},
#line 313 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"obsolete",	x86_cpu_set,	CPU_Obs},
#line 289 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"sse2",		x86_cpu_set,	CPU_SSE2},
#line 304 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"noprot",		x86_cpu_clear,	CPU_Prot},
#line 302 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nosmm",		x86_cpu_clear,	CPU_SMM},
#line 240 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"p2",		x86_cpu_intel,	PROC_p2},
#line 292 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nosse3",		x86_cpu_clear,	CPU_SSE3},
#line 267 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"athlon64",	x86_cpu_amd,	PROC_hammer},
#line 268 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"athlon-64",	x86_cpu_amd,	PROC_hammer},
#line 338 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nosse4",		x86_cpu_clear_sse4,	0},
#line 325 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"ssse3",		x86_cpu_set,	CPU_SSSE3},
#line 256 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"williamette",	x86_cpu_intel,	PROC_p4},
#line 285 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"mmx",		x86_cpu_set,	CPU_MMX},
#line 262 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"athlon",		x86_cpu_amd,	PROC_k7},
#line 235 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"686",		x86_cpu_intel,	PROC_686},
#line 231 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"586",		x86_cpu_intel,	PROC_586},
#line 271 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"bulldozer",	x86_cpu_amd,	PROC_bulldozer},
#line 341 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"xsave",		x86_cpu_set,	CPU_XSAVE},
#line 261 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"k7",		x86_cpu_amd,	PROC_k7},
#line 339 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"sse5",		x86_cpu_set,	CPU_SSE5},
#line 260 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"k6",		x86_cpu_amd,	PROC_k6},
#line 230 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"i486",		x86_cpu_intel,	PROC_486},
#line 342 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"noxsave",	x86_cpu_clear,	CPU_XSAVE},
#line 233 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentium",	x86_cpu_intel,	PROC_586},
#line 320 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nosvm",		x86_cpu_clear,	CPU_SVM},
#line 310 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"noundocumented",	x86_cpu_clear,	CPU_Undoc},
#line 221 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"i186",		x86_cpu_intel,	PROC_186},
#line 326 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nossse3",	x86_cpu_clear,	CPU_SSSE3},
#line 283 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"fpu",		x86_cpu_set,	CPU_FPU},
#line 236 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"i686",		x86_cpu_intel,	PROC_686},
#line 344 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"noavx",		x86_cpu_clear,	CPU_AVX},
#line 340 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nosse5",		x86_cpu_clear,	CPU_SSE5},
#line 298 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nocyrix",	x86_cpu_clear,	CPU_Cyrix},
#line 254 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentiumiv",	x86_cpu_intel,	PROC_p4},
#line 258 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"ia-64",		x86_cpu_ia64,	0},
#line 312 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"noobs",		x86_cpu_clear,	CPU_Obs},
#line 259 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"itanium",	x86_cpu_ia64,	0},
#line 228 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"486",		x86_cpu_intel,	PROC_486},
#line 242 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentium-2",	x86_cpu_intel,	PROC_p2},
#line 297 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"cyrix",		x86_cpu_set,	CPU_Cyrix},
#line 276 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nehalem",	x86_cpu_intel,	PROC_nehalem},
#line 247 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"pentium-3",	x86_cpu_intel,	PROC_p3},
#line 347 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"aes",		x86_cpu_set,	CPU_AES},
#line 322 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nopadlock",	x86_cpu_clear,	CPU_PadLock},
#line 305 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"protected",	x86_cpu_set,	CPU_Prot},
#line 251 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"p4",		x86_cpu_intel,	PROC_p4},
#line 237 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"p6",		x86_cpu_intel,	PROC_686},
#line 299 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"amd",		x86_cpu_set,	CPU_AMD},
#line 336 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nosse4a",	x86_cpu_clear,	CPU_SSE4a},
#line 284 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"nofpu",		x86_cpu_clear,	CPU_FPU},
#line 349 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"clmul",		x86_cpu_set,	CPU_CLMUL},
#line 331 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"
    {"sse4.2",		x86_cpu_set,	CPU_SSE42}
  };
  static const unsigned char tab[] = {
    125,113,40,0,113,113,0,0,0,113,0,7,0,0,113,0,
    125,82,0,0,0,125,0,0,113,125,113,125,0,0,0,125,
    113,0,0,0,7,7,7,0,0,125,7,0,82,0,0,0,
    40,0,0,0,0,124,0,113,0,27,0,0,0,0,0,0,
    82,85,113,0,0,88,120,0,7,0,0,0,0,0,0,0,
    40,125,0,0,113,0,7,0,0,0,0,0,120,0,113,113,
    0,0,125,27,51,113,0,0,0,0,58,116,125,124,0,128,
    0,125,27,0,0,0,0,0,111,82,0,119,82,0,0,0,
  };

  const struct cpu_parse_data *ret;
  unsigned long rsl, val = phash_lookup(key, len, 0xdaa66d2bUL);
  rsl = ((val>>25)^tab[val&0x7f]);
  if (rsl >= 129) return NULL;
  ret = &pd[rsl];
  if (strcmp(key, ret->name) != 0) return NULL;
  return ret;
}

#line 353 "/home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/./pkgs/tools/yasm/src/modules/arch/x86/x86cpu.gperf"


void
yasm_x86__parse_cpu(yasm_arch_x86 *arch_x86, const char *cpuid,
                    size_t cpuid_len)
{
    /*@null@*/ const struct cpu_parse_data *pdata;
    wordptr new_cpu;
    size_t i;
    static char lcaseid[16];

    if (cpuid_len > 15)
        return;
    for (i=0; i<cpuid_len; i++)
        lcaseid[i] = tolower(cpuid[i]);
    lcaseid[cpuid_len] = '\0';

    pdata = cpu_find(lcaseid, cpuid_len);
    if (!pdata) {
        yasm_warn_set(YASM_WARN_GENERAL,
                      N_("unrecognized CPU identifier `%s'"), cpuid);
        return;
    }

    new_cpu = BitVector_Clone(arch_x86->cpu_enables[arch_x86->active_cpu]);
    pdata->handler(new_cpu, pdata->data);

    /* try to find an existing match in the CPU table first */
    for (i=0; i<arch_x86->cpu_enables_size; i++) {
        if (BitVector_equal(arch_x86->cpu_enables[i], new_cpu)) {
            arch_x86->active_cpu = i;
            BitVector_Destroy(new_cpu);
            return;
        }
    }

    /* not found, need to add a new entry */
    arch_x86->active_cpu = arch_x86->cpu_enables_size++;
    arch_x86->cpu_enables =
        yasm_xrealloc(arch_x86->cpu_enables,
                      arch_x86->cpu_enables_size*sizeof(wordptr));
    arch_x86->cpu_enables[arch_x86->active_cpu] = new_cpu;
}
