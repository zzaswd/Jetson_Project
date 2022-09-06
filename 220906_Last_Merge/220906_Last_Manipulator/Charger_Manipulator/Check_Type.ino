//==================================
// 차종에 따라 충전구 선택 및 자리 선택
//==================================

void Check_Type() {
  const char *log;
  int connecterTy;

  connecterTy = 2;

  if (connecterTy == 1) {
    dxl_wb.goalPosition(id_list[0], (int32_t)Degree(180));
    dxl_wb.goalPosition(id_list[1], (int32_t)Degree(270));
    dxl_wb.goalPosition(id_list[2], (int32_t)Degree(270));
    dxl_wb.goalPosition(id_list[3], (int32_t)Degree(180));

    move_horn(0, 180, 1000, &log);
    move_horn(1, 180, 1000, &log);

    Serial.printf("This is Not Electric Car... Wait for Charging");

    //if (connecterTy != 1)
    //break;
  } // type1 -> Not Electric Car
  else if (connecterTy == 2) {
    move_horn(0, 215, 1000, &log);
    move_horn(1, 145, 1000, &log);
    dxl_wb.goalPosition(id_list[2], (int32_t)Degree(215));

    delay(1000);

    dxl_wb.goalPosition(id_list[2], (int32_t)Degree(90));
    delay(300);

    dxl_wb.goalPosition(id_list[3], (int32_t)Degree(90));
    delay(300);
    
    move_horn(1, 180, 1000, &log);
    move_horn(0, 180, 1000, &log);
  } //electric car connecter type 2
  else if (connecterTy == 3) {
    move_horn(0, 215, 1000, &log);
    move_horn(1, 145, 1000, &log);
    dxl_wb.goalPosition(id_list[2], (int32_t)Degree(215));
    

    delay(1000);

    dxl_wb.goalPosition(id_list[2], (int32_t)Degree(270));
    delay(300);
    move_horn(1, 180, 1000, &log);
    move_horn(0, 180, 1000, &log);
  } //electric car connecter type 3
}
