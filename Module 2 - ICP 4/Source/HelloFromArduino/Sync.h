#pragma once

class Sync {
  public:
  Sync();
  Sync(unsigned long start);
  bool elapsed(unsigned long timeout);
  void reset();
  private:
  unsigned long previous;
};
