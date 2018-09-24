#pragma once

class Sync {
  public:
  Sync();
  bool elapsed(unsigned long timeout);
  bool elapsedMicroseconds(unsigned long timeout);
  void reset();
  private:
  unsigned long previous;
};
