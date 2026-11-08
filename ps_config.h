
#define _ps_config_define(name, type, value)
/* this macro breaks styling guidelines to allow easier usage */
#define config_define(name, type, value) _ps_config_define(name, type, value)

#ifndef __i_ps_cfg_stringify
#define __i_ps_cfg_stringify0(name) #name
#define __i_ps_cfg_stringify(name) __i_ps_cfg_stringify0(name)
#endif /* __i_ps_cfg_stringify */

#ifndef __i_ps_cfg_concat
#define __i_ps_cfg_concat0(a, b) a ## b
#define __i_ps_cfg_concat(a, b) __i_ps_cfg_concat0(a, b)
#endif /* __i_ps_cfg_concat */

/* PS_CONFIG_NAME breaks styling guidelines to allow easier usage */
#ifdef PS_STRIP_PREFIX
#ifdef CONFIG_NAME
#define PS_CONFIG_NAME CONFIG_NAME
#endif /* CONFIG_NAME */
#endif /* PS_CONFIG_NAME */
#ifdef PS_CONFIG_NAME
#undef _ps_config_define
#define __I_PS_CONFIG_FILE __i_ps_cfg_stringify(PS_CONFIG_NAME) ".h"
#define __I_PS_CONFIG_TYPE_NAME __i_ps_cfg_concat(PS_CONFIG_NAME, _t)
#define __I_PS_CONFIG_INSTANCE_NAME __i_ps_cfg_concat(PS_CONFIG_NAME, _cfg)


#define _ps_config_define(name, type, value) type name;
typedef struct {
    #include __I_PS_CONFIG_FILE
} __I_PS_CONFIG_TYPE_NAME;
#undef _ps_config_define

/* PS_CONFIG_IMP breaks styling guidelines to allow easier usage */
#ifdef PS_CONFIG_IMP
#define _ps_config_define(name, type, value) value,
const __I_PS_CONFIG_TYPE_NAME __I_PS_CONFIG_INSTANCE_NAME = {
    #include __I_PS_CONFIG_FILE
};
#undef _ps_config_define
#else
    extern const __I_PS_CONFIG_TYPE_NAME __I_PS_CONFIG_INSTANCE_NAME;
#endif /* PS_CONFIG_IMP */

#undef __I_PS_CONFIG_FILE
#undef __I_PS_CONFIG_TYPE_NAME
#undef __I_PS_CONFIG_INSTANCE_NAME
#undef PS_CONFIG_NAME
#ifdef PS_STRIP_PREFIX
#undef CONFIG_NAME
#endif /* PS_STRIP_PREFIX */
#undef PS_CONFIG_IMP

#define _ps_config_define(name, type, value)
#endif /* PS_CONFIG_NAME */

#undef __i_ps_cfg_stringify0
#undef __i_ps_cfg_stringify
#undef __i_ps_cfg_concat0
#undef __i_ps_cfg_concat
