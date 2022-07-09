#ifndef __BEEP_H__
#define __BEEP_H__

enum BEEP_STATUS {
	KEEP_SILENT,
	ONE_SOUND,
	TWO_SOUND,
	THREE_SOUND,
	ALWAYS_SOUND
};

#define BEEP_Pin GPIO_Pin_0
#define BEEP_GPIO_Port GPIOC


#endif /* __BEEP__ */
