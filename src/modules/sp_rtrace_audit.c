#define _GNU_SOURCE

#include <link.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <execinfo.h>

#include "sp_rtrace_module.h"
#include "sp_rtrace_main.h"
#include "common/sp_rtrace_proto.h"
#include "library/sp_rtrace_defs.h"
#include "library/sp_rtrace_tracker.h"
#include "rtrace/rtrace_env.h"

static sp_rtrace_tracker_t tracker;

//static unsigned int context_mask = 0;

/**
 * Module information
 */
static sp_rtrace_module_info_t module_info = {
		.type = MODULE_TYPE_AUDIT,
		.version_major = 1,
		.version_minor = 0,
		.name = "audit",
		.description = "Custom function call tracking module based on dynamic linker audit feature."
};

static bool is_tracking_enabled = false;

static unsigned int backtrace_depth = 10;

/* virtual resource size/id */
#define RES_SIZE		1
#define RES_ID			1

/**
 * Enables/disables tracing.
 *
 * @param[in] value   enable tracing if true, otherwise disable it.
 * @return
 */
static void enable_tracing(bool value)
{
	is_tracking_enabled = value;
}

/**
 * virtual audit resource
 */
static sp_rtrace_resource_t res_audit = {
		.type = "audit",
		.desc = "virtual audit resource",
		.flags = SP_RTRACE_RESOURCE_DEFAULT,
};


unsigned int la_version(unsigned int version)
{
	return version;
}

unsigned int la_objopen(
		struct link_map *l,
		Lmid_t a  __attribute__((unused)),
		uintptr_t *cookie __attribute__((unused))
	)
{
	if (l->l_name && *l->l_name) {
		sp_rtrace_write_new_library(l->l_name);
	}
	return LA_FLG_BINDTO | LA_FLG_BINDFROM;
}

uintptr_t la_symbind32(
		Elf32_Sym *sym,
		unsigned int ndx  __attribute__((unused)),
        uintptr_t *refcook __attribute__((unused)),
        uintptr_t *defcook  __attribute__((unused)),
        unsigned int *flags,
        const char *symname
	)
{
	if (sp_rtrace_tracker_query_symbol(&tracker, symname)) {
		*flags |= LA_SYMB_NOPLTEXIT;
	}
	else {
		*flags |= LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT;
	}
	return sym->st_value;
}

#ifdef __x86_64__

uintptr_t la_symbind64(
		Elf64_Sym *sym,
		unsigned int ndx  __attribute__((unused)),
        uintptr_t *refcook __attribute__((unused)),
        uintptr_t *defcook  __attribute__((unused)),
        unsigned int *flags,
        const char *symname
	)
{
	if (sp_rtrace_tracker_query_symbol(&tracker, symname)) {
		*flags |= LA_SYMB_NOPLTEXIT;
	}
	else {
		*flags |= LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT;
	}
	return sym->st_value;
}

# define pltenter la_x86_64_gnu_pltenter
# define pltexit la_x86_64_gnu_pltexit
# define La_regs La_x86_64_regs
# define La_retval La_x86_64_retval

// number of 'internal' stack frames that should be skipped
#define TOP		2

/**
 * Retrieves current stack trace.
 *
 * @param[in] frames   the output buffer.
 * @param[in] length   the number of frames to retrieve.
 * @param[in] regs     the registers before function call
 * @return             a reference to the first stack frame in the output buffer.
 */
static pointer_t* backtrace_audit(pointer_t* frames, unsigned int* nframes, La_regs *regs)
{
	unsigned int length = *nframes > backtrace_depth + TOP ? backtrace_depth + TOP : *nframes;
	*nframes = backtrace((void**)frames, length);

	if (*nframes <= TOP) {
		*nframes = 0;
		return frames;
	}
	*nframes -= TOP;
	frames += TOP;
	frames[0] =  *(pointer_t*)regs->lr_rsp;
	return frames;
}

#endif

#ifdef __i386__
# define pltenter la_i86_gnu_pltenter
# define pltexit la_i86_gnu_pltexit
# define La_regs La_i86_regs
# define La_retval La_i86_retval

// number of 'internal' stack frames that should be skipped
#define TOP		2

/**
 * Retrieves current stack trace.
 *
 * @param[in] frames   the output buffer.
 * @param[in] length   the number of frames to retrieve.
 * @param[in] regs     the registers before function call
 * @return             a reference to the first stack frame in the output buffer.
 */
static pointer_t* backtrace_audit(pointer_t* frames, unsigned int* nframes, La_regs *regs)
{
	unsigned int length = *nframes > backtrace_depth + TOP ? backtrace_depth + TOP : *nframes;
	*nframes = backtrace((void**)frames, length);
	if (*nframes <= TOP) {
		*nframes = 0;
		return frames;
	}
	*nframes -= TOP;
	frames += TOP;
	frames[0] =  *(pointer_t*)regs->lr_esp;
	return frames;
}

#elif defined __arm__
# define pltenter la_arm_gnu_pltenter
# define pltexit la_arm_gnu_pltexit
# define La_regs La_arm_regs
# define La_retval La_arm_retval

// number of 'internal' stack frames that should be skipped
#define TOP		3

/**
 * Retrieves current stack trace.
 *
 * @param[in] frames   the output buffer.
 * @param[in] length   the number of frames to retrieve.
 * @param[in] regs     the registers before function call
 * @return             a reference to the first stack frame in the output buffer.
 */
static pointer_t* backtrace_audit(pointer_t* frames, unsigned int* nframes, La_regs *regs)
{
	unsigned int length = *nframes > backtrace_depth + TOP ? backtrace_depth + TOP : *nframes;
	*nframes = backtrace((void**)frames, length);
	if (*nframes <= TOP) {
		*nframes = 0;
		return frames;
	}
	*nframes -= TOP;
	frames += TOP;
	frames[0] =  (pointer_t)regs->lr_lr;
	return frames;
}

#endif

ElfW(Addr)
pltenter(
	ElfW(Sym) *sym,
	unsigned int ndx  __attribute__((unused)),
	uintptr_t *refcook  __attribute__((unused)),
	uintptr_t *defcook __attribute__((unused)),
	La_regs *regs  __attribute__((unused)),
	unsigned int *flags  __attribute__((unused)),
    const char *symname  __attribute__((unused)),
    long int *framesizep  __attribute__((unused))
	)
{
	if (is_tracking_enabled) {
		char* symbol = sp_rtrace_tracker_query_symbol(&tracker, symname);
		if (symbol) {
			sp_rtrace_fcall_t call = {
					.type = SP_RTRACE_FTYPE_ALLOC,
					.res_type = (void*)(long)res_audit.id,
					.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
					.name = (char*)symbol,
					.res_size = RES_SIZE,
					.res_id = (pointer_t)RES_ID,
			};

			pointer_t frames[256];
			sp_rtrace_ftrace_t trace = {
				.nframes = 0,
				.frames = NULL,
			};
			if (backtrace_depth) {
				trace.nframes = sizeof(frames) / sizeof(frames[0]);
				trace.frames = backtrace_audit(frames, &trace.nframes, regs);
			}
			sp_rtrace_write_function_call(&call, &trace, NULL);
			free(symbol);
		}
	}
	return sym->st_value;
}


static void trace_audit_init(void) __attribute__((constructor));
static void trace_audit_fini(void) __attribute__((destructor));

static void trace_audit_init(void)
{
	const char* env_backtrace_depth = getenv(rtrace_env_opt[OPT_BACKTRACE_DEPTH]);
	if (env_backtrace_depth && *env_backtrace_depth) {
		backtrace_depth = atoi(env_backtrace_depth);
	}

	sp_rtrace_tracker_init(&tracker, getenv(rtrace_env_opt[OPT_AUDIT]));

	sp_rtrace_register_module(module_info.name, module_info.version_major, module_info.version_minor, enable_tracing);
	sp_rtrace_register_resource(&res_audit);
}

static void trace_audit_fini(void)
{
	enable_tracing(false);
}


