
#ifndef TRANTOR_EXPORT_H
#define TRANTOR_EXPORT_H

#ifdef TRANTOR_STATIC_DEFINE
#  define TRANTOR_EXPORT
#  define TRANTOR_NO_EXPORT
#else
#  ifndef TRANTOR_EXPORT
#    ifdef trantor_EXPORTS
        /* We are building this library */
#      define TRANTOR_EXPORT 
#    else
        /* We are using this library */
#      define TRANTOR_EXPORT 
#    endif
#  endif

#  ifndef TRANTOR_NO_EXPORT
#    define TRANTOR_NO_EXPORT 
#  endif
#endif

#ifndef TRANTOR_DEPRECATED
#  define TRANTOR_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef TRANTOR_DEPRECATED_EXPORT
#  define TRANTOR_DEPRECATED_EXPORT TRANTOR_EXPORT TRANTOR_DEPRECATED
#endif

#ifndef TRANTOR_DEPRECATED_NO_EXPORT
#  define TRANTOR_DEPRECATED_NO_EXPORT TRANTOR_NO_EXPORT TRANTOR_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef TRANTOR_NO_DEPRECATED
#    define TRANTOR_NO_DEPRECATED
#  endif
#endif

#endif /* TRANTOR_EXPORT_H */
