#include "pem_moa_msg.h"



#define DO_MESSAGE_KEY_STR(c) #c,
static const char* messages_str[] = {
	"Null Msg", /* NULL */
	UTIL_MESSAGE_LIST ( DO_MESSAGE_KEY_STR )
	"Last Msg"  /* LAST */
};
#undef DO_MESSAGE_KEY_STR


/**
 *\brief <brief description>
 *
 * <detailed description>
 *
 * \param[in] index <description>
 * \return <info about return value>
 */
const char* getMessageStr(MSG_MOA_TYPE_T index) {
	return messages_str[index];
}
