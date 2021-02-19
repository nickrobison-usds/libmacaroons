#ifdef PARANOID_MACAROONS
#define VALIDATE(M) assert(macaroon_validate(M) == 0);
#else
#define VALIDATE(M) do {} while (0)
#endif
#ifdef __GNUC__
#define MACAROON_API __attribute__ ((visibility ("default")))
#else
#define MACAROON_API
#endif

#ifdef __GNUC__
#define WEAK __attribute__((weak))
#else
#define WEAK
#endif
