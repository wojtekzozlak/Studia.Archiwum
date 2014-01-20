/*
 * Autor: Osowski Marcin
 * nr indeksu: 292682
 *
 * Trzecie zadanie z Systemów Operacyjnych (2010/11)
 *
 */

#ifndef _VT100_H_
#define _VT100_H_


#ifdef ENABLE_VT100

#define VT100_BOLD          "\033[1m"
#define VT100_RED           "\033[31m"
#define VT100_MAGENTA       "\033[35m"
#define VT100_NOATTR        "\033[m"

#else /* ENABLE_VT100 */

#define VT100_BOLD          ""
#define VT100_RED           ""
#define VT100_MAGENTA       ""
#define VT100_NOATTR        ""

#endif /* ENABLE_VT100 */


#endif /* _VT100_H_ */
