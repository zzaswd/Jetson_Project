//=======================
// 2축 다이나믹셀 동시 동작 불가 문제 때문에
// 다음과 같이 함수화
// 움직이고 torque를 해제하는 것.
//=======================
void move_horn(int idx, int angle, int delay_time,const char **l){
  dxl_wb.jointMode(id_list_2horn[idx], 0, 0, l); //조인트모드 설정
  dxl_wb.goalPosition(id_list_2horn[idx], (int32_t)Degree(angle)); 
  delay(delay_time);    //반드시 딜레이를 줘야함 -> 원하는 각도 미 도달후 움직일 시 동작 정지
  dxl_wb.itemWrite(id_list_2horn[idx], "Torque_Enable", 0, l);
}
