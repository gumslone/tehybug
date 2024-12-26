class IntervalTimer{
  public:
  IntervalTimer(uint16_t interval): m_interval{interval} {}
  bool expired(bool updateLast = false)
  {
    if (millis() - m_lastTime >= m_interval) {
      if(updateLast)
      {
        updateLast();
      }
      return true;
    } 
    return false; 
  }
  bool updateLast()
  {
    m_lastTime = millis();
  }
  private:
  uint16_t m_interval{1000};
  uint32_t m_lastTime{0};
};
