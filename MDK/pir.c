void pir_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//???????
    
    RCC_APB2PeriphClockCmd(pir_RCC,ENABLE);	
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin =pir_PIN ;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(pir_PORT ,&GPIO_InitStructure);
	
}
