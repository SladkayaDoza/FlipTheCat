void viewer(uint16_t point) {
  bool displayUpdate = 1;
  
  while(1) {
    static uint8_t pointer = 0;
    tk();
    if (displayUpdate != 0) {
      displayUpdate = 0;
      oled.clear();
      oled.home();

      
      oled.update();
    }

    if (up.click() or up.step()) {
      ;;
    }
    if (down.click() or down.step()) {
      ;;
    }
    if (left.click() or left.step()) {
      ;;
    }
    if (right.click() or right.click()) {
      ;;
    }
    if (back.click()) return;
  }
}