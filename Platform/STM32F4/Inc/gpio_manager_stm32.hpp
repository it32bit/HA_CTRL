#include "i_gpio.hpp"
#include <vector>
#include <memory>

class GpioManager_STM32 : public IGPIOManager
{
  public:
    GpioManager_STM32();

    IGPIOPin* getPin(std::string_view pinName) override;

    // Initialize pins from config (existing config array)
    void initializePins(const std::vector<IOD>& pinConfigs);

  private:
    std::vector<std::unique_ptr<GpioPin_STM32>> m_pins;
};
