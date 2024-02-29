// void write_eeprom() {
//   int address = 1;

//   for (uint8_t i=0; i<RC_ITEMS; i++) {
//     EEPROM.writeULong(address, save_data.data_prog[i]); 
//     address += sizeof(long);
//   }
//   for (uint8_t i=0; i<RC_ITEMS; i++) {
//     EEPROM.writeByte(address, save_data.data_prog1[i]); 
//     address += sizeof(byte);
//   }

//   EEPROM.commit();
// }


// void read_eeprom() {
//   int address = 1;

//   for (uint8_t i=0; i<RC_ITEMS; i++) {
//     save_data.data_prog[i] = EEPROM.readULong(address); 
//     address += sizeof(long);
//   }
//   for (uint8_t i=0; i<RC_ITEMS; i++) {
//     save_data.data_prog1[i] = EEPROM.readByte(address); 
//     address += sizeof(byte);
//   }
//   for (uint8_t i=0; i<RC_ITEMS; i++) {
//     if (save_data.data_prog[i] != 0) {
//       snprintf(trans_lay[i], colls, "%lu - %hu", save_data.data_prog[i], save_data.data_prog1[i]);
//     }
//   }
// }