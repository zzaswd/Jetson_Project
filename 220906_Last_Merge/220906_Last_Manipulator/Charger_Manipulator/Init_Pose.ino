//=======================
// 대기 상태 Pose 위치.
//=======================

void Init_First () { 

  const char *log;
  int connecterTy;
  
  dxl_wb.goalPosition(id_list[0], (int32_t)Degree(180));
  dxl_wb.goalPosition(id_list[1], (int32_t)Degree(270));
  dxl_wb.goalPosition(id_list[2], (int32_t)Degree(270));
  dxl_wb.goalPosition(id_list[3], (int32_t)Degree(180));

  //초기상태 지정....//

  move_horn(0, 180, 1000, &log);
  move_horn(1, 180, 1000, &log);

  Serial.printf("Baisc Postion Setting FINISH");

  connecterTy = 2; //(1번 차종일 경우)
}
