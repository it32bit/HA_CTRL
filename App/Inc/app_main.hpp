/**
 ******************************************************************************
 * @file        : app_main.hpp
 * @author      : i32bit
 * @brief       : Header for app_main.cpp file.
 * @details     : This file contains the common defines of the application.
 ******************************************************************************
 * This software is licensed under the MIT License.
 * Provided "as is", without warranty of any kind.
 * The author is not liable for any damages resulting from its use.
 ******************************************************************************
 */
#ifndef __APP_MAIN_HPP
#define __APP_MAIN_HPP

#ifdef __cplusplus

class Debouncer
{
  public:
    Debouncer(uint32_t debounceMs) : debounceTime(debounceMs), lastTick(0), locked(false) {}

    bool shouldTrigger()
    {
        uint32_t now = HAL_GetTick();

        if (now - lastTick > debounceTime)
        {
            lastTick = now;
            locked   = false;
        }

        if (locked == false)
        {
            locked = true;
            return true;
        }

        return false;
    }

  private:
    uint32_t debounceTime;
    uint32_t lastTick;
    bool     locked;
};

extern "C"
{
#endif

    void HeartBeat_SysTick(void);
    void App_cpp(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_MAIN_HPP */
