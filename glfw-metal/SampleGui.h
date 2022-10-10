#include "GuiInterface.h"


class SampleGui : public GuiInterface {
  using GuiInterface::GuiInterface;
  virtual void Render() override;
};
