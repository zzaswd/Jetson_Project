//=======================
// Angle Topic 기준으로 각도제어
//=======================

void Set_Complete_angle_x(string angle1, string angle2) {
  std::stringstream temp1;
  std::stringstream temp2;

  temp1.str(angle1);
  temp2.str(angle2);

  int ang1, ang2;

  temp1 >> ang1;
  temp2 >> ang2;

  Serial.print("Set_X : angle1 ="); Serial.print(ang1); Serial.print("  angle2 = "); Serial.println(ang2);

  const char *log;
  if (ang1 > 0) {
    for (int i = 0; i < ang1; i++)
    {
      dxl_wb.goalPosition(id_list[0], (int32_t)Degree(180 - i));
      delay(100);
    }
  }
  else {
    for (int i = 0; i > ang1; i--)
    {
      dxl_wb.goalPosition(id_list[0], (int32_t)Degree(180 - i));
      delay(100);
    }
  }

  if (ang2 > 0) {
    for (int i = 0; i < ang2; i++)
    {
      move_horn(0, (180 - i), 100, &log);
    }
  }
  else {
    for (int i = 0; i > ang2; i--)
    {
      move_horn(0, (180 - i), 100, &log);
    }
  }

}


void Set_Complete_angle_y(string angle3, string angle4) {
  const char *log;

  std::stringstream temp3;
  std::stringstream temp4;

  temp3.str(angle3);
  temp4.str(angle4);

  int ang3, ang4;

  temp3 >> ang3;
  temp4 >> ang4;

  Serial.print("Set_Y : angle3 ="); Serial.print(ang3); Serial.print("  angle4 = "); Serial.println(ang4);

  if (ang3 > 0) {
    for (int i = 0; i < ang3; i++)
    {
      move_horn(1, (180 + i), 100, &log);
    }
  }
  else {
    for (int i = 0; i > ang3; i--)
    {
      move_horn(1, (180 + i), 100, &log);
    }
  }

  if (ang4 > 0) {
    for (int i = 0; i < ang4; i++)
    {
      dxl_wb.goalPosition(id_list[2], (int32_t)Degree(90 +  i));
      delay(100);
    }
  }
  else {
    for (int i = 0; i > ang4; i--)
    {
      dxl_wb.goalPosition(id_list[2], (int32_t)Degree(90 +  i));
      delay(100);
    }
  }
  if (ang3 + ang4 > 0) {
    for (int i = 0; i < ang3 + ang4; i++)
    {
      dxl_wb.goalPosition(id_list[3], (int32_t)Degree(90 -  i));
      delay(100);
    }
  }
  else {
    for (int i = 0; i > ang3 + ang4; i--)
    {
      dxl_wb.goalPosition(id_list[3], (int32_t)Degree(90 -  i));
      delay(100);
    }
  }
}
