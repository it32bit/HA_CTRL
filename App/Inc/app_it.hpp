#ifndef APP_IT_HPP__
#define APP_IT_HPP__

#ifdef __cplusplus



extern "C"
{
#endif

    void HeartBeat_SysTick(void);
    void EXTI0_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif //APP_IT_HPP__
