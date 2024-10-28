本文件夹用于存放：
queryAgtim.c是queryA捕获query信号(5ms),queryB输出query信号
queryBgtim.c是queryB捕获query信号(5ms),queryA输出query信号
PB3	= QUERY_INB      TIM2_CH2 AF1 (5MS周期)
PA15n=	QUERY_INA  TIM2_CH1 AF1

PA9	= MCU_CT_QUERYB
PA8	= MCU_CT_QUERYA








