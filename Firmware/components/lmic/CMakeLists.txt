set(COMPONENT_SRCS "lmic.c"
                    "hal.c"
                    "oslmic.c"
                    "radio.c"
                    "aes.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "../eid430/include"
                               "../pca9537/include")
set(COMPONENT_REQUIRES)
message("-> build Lmic...")
register_component()