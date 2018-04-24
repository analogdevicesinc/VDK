#ifndef _AUDIOMSG_H_
#define _AUDIOMSG_H_

#define CONTROL_CHANNEL     VDK::kMsgChannel1  // highest priority
#define SEND_BUFF_CHANNEL   VDK::kMsgChannel7  // mid priority
#define RETURN_BUFF_CHANNEL VDK::kMsgChannel15 // lower priority

enum MsgType
{
	PCM_BUFFER_8,
	PCM_BUFFER_16,
	PCM_BUFFER_24,
	PCM_BUFFER_32,
	VOL_CHANGE
};

#define SAMPLEPAIRS_PER_BUFFER 48

#endif /* _AUDIOMSG_H_ */

