#ifndef __I_PS_CFG_INTER_
#       ifndef __i_ps_stringify
#               define __i_ps_stringify0(x) #x
#               define __i_ps_stringify(x) __i_ps_stringify0(x)
#       endif /* __i_ps_stringify */

#       ifndef __i_ps_concat
#               define __i_ps_concat0(x, y) x##y
#               define __i_ps_concat(x, y) __i_ps_concat0(x, y)
#       endif /* __i_ps_concat */

#       ifndef _ps_config_define
#               define _ps_config_define(name, type, value)
#       endif /* _ps_config_define */
#       ifdef PS_STRIP_PREFIX
#               ifdef CONFIG_NAME
#                       define PS_CONFIG_NAME CONFIG_NAME
#               endif /* CONFIG_NAME */
#               ifdef CONFIG_FILE
#                       define PS_CONFIG_FILE CONFIG_FILE
#               endif /* CONFIG_FILE */
#               ifdef CONFIG_IMP
#                       define PS_CONFIG_IMP CONFIG_IMP
#               endif /* CONFIG_IMP */
#               ifdef DEFAULT_CONFIG_FILE
#                       define PS_DEFAULT_CONFIG_FILE DEFAULT_CONFIG_FILE
#               endif /* DEFAULT_CONFIG_FILE */
#               ifdef DEFAULT_CONFIG
#                       define PS_DEFAULT_CONFIG DEFAULT_CONFIG
#               endif /* DEFAULT_CONFIG */

#       endif /* PS_STRIP_PREFIX */

#       ifdef PS_CONFIG_NAME
#               undef _ps_config_define
#               ifndef PS_DEFAULT_CONFIG_FILE
#                       define PS_DEFAULT_CONFIG_FILE PS_CONFIG_FILE
#               endif /* PS_DEFAULT_CONFIG_FILE */
#               define PS_CONFIG_TYPE_NAME __i_ps_concat(PS_CONFIG_NAME, _t)
#               define PS_CONFIG_INSTANCE_NAME PS_CONFIG_NAME

#               define _ps_config_define(name, type, value) type name;
                typedef struct {
#                       define __I_PS_CFG_INTER_
#                       include PS_DEFAULT_CONFIG_FILE
#                       undef PS_DEFAULT_CONFIG
#                       ifdef PS_STRIP_PREFIX
#                               undef DEFAULT_CONFIG
#                       endif /* PS_STRIP_PREFIX */
#                       undef __I_PS_CFG_INTER_        
                } PS_CONFIG_TYPE_NAME;
#               undef _ps_config_define
#               ifdef PS_CONFIG_IMP
#                       define _ps_config_define(name, type, value) .name = value,
                        PS_CONFIG_TYPE_NAME PS_CONFIG_INSTANCE_NAME = {
#                               define __I_PS_CFG_INTER_
#                                       include PS_DEFAULT_CONFIG_FILE
#                                       undef PS_DEFAULT_CONFIG
#                                       ifdef PS_STRIP_PREFIX
#                                               undef DEFAULT_CONFIG
#                                       endif /* PS_STRIP_PREFIX */
#                                       include PS_CONFIG_FILE
#                               undef __I_PS_CFG_INTER_
                        };
#                       undef _ps_config_define
#               else
                        extern PS_CONFIG_TYPE_NAME PS_CONFIG_INSTANCE_NAME;
#               endif /* PS_CONFIG_IMP */
#       endif /* PS_CONFIG_INSTANCE_NAME */
#       define _ps_config_define(name, type, value)
#       undef PS_CONFIG_NAME
#       undef PS_CONFIG_FILE
#       undef PS_DEFAULT_CONFIG_FILE
#       undef PS_CONFIG_TYPE_NAME
#       undef PS_CONFIG_INSTANCE_NAME
#       undef PS_CONFIG_IMP
#       undef PS_DEFAULT_CONFIG
#       undef ps_config_define
#       undef ps_config_set
#       ifdef PS_STRIP_PREFIX
#               undef CONFIG_NAME
#               undef CONFIG_IMP
#               undef CONFIG_FILE
#               undef DEFAULT_CONFIG_FILE
#               undef DEFAULT_CONFIG
#               undef config_define
#               undef config_set
#       endif /* PS_STRIP_PREFIX */
#       undef __i_ps_stringify0
#       undef __i_ps_stringify
#       undef __i_ps_concat0
#       undef __i_ps_concat
#endif /* PS_CONFIG_TYPE_NAME */

#ifdef PS_STRIP_PREFIX
#   ifdef DEFAULT_CONFIG
#       ifndef PS_DEFAULT_CONFIG
#           define PS_DEFAULT_CONFIG
#       endif /* PS_DEFAULT_CONFIG */
#   endif /* DEFAULT_CONFIG */

#endif /* PS_STRIP_PREFIX */

#ifndef ps_config_define
#   if defined(_ps_config_define) && defined(PS_DEFAULT_CONFIG)
#       define ps_config_define(name, type, value) _ps_config_define(name, type, value)
#   elif defined(PS_DEFAULT_CONFIG)
#       define ps_config_define(name, type, value)
#   endif /* _ps_config_define */
#endif /* ps_config_define */

#ifndef ps_config_set
#   if defined(_ps_config_define)
#       define ps_config_set(name, value) _ps_config_define(name, void, value)
#   else
#       define ps_config_set(name, value)
#   endif /* _ps_config_define */
#endif /* ps_config_set */

#ifdef PS_STRIP_PREFIX
#   define config_define ps_config_define
#   define config_set ps_config_set
#endif /* PS_STRIP_PREFIX */

