void printPointer(uint8_t ptr) {
  ptr = ptr-((ptr/8)*8);
  oled.setCursor(0, ptr);
  oled.print(">");
  // oled.update(0, ptr*8-8, 7, ptr*8+15);
}

void printRightPointer(uint8_t ptr) {
  ptr = ptr-((ptr/8)*8);
  oled.setCursor(120, ptr);
  oled.print("<");
  // oled.update(0, ptr*8-8, 7, ptr*8+15);
}