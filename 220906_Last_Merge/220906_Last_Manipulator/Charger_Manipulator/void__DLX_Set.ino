//=======================
// Dynamixel 초기화 과정
// position mode랑 torque on 진행
//=======================

void DLX_Set(void){ //초기화 함수 시작
  const char *log;
  uint16_t model_number = 0;
  
  // 동작 안될 때 확인용
  result = dxl_wb.init(DEVICE_NAME, BAUDRATE, &log); //초기화

  if (result == false){
    Serial.println(log);
    Serial.println("Failed to init");
  }
  else{
    Serial.print("Succeeded to init : ");
    Serial.println(BAUDRATE);  
  }
  
  for(int idx = 0; idx < sizeof(id_list)/sizeof(char);idx++){
      result = dxl_wb.ping(id_list[idx], &model_number, &log); //각 아이디에게 핑을 보낸다(상태 확인)
      if (result == false){
        Serial.println(log);
        Serial.println("Failed to ping");
      }
      else {
        Serial.println("Succeeded to ping");
        Serial.print("id : ");
        Serial.print(id_list[idx]);
        Serial.print(" model_number : ");
        Serial.println(model_number);
      }
  }

  for(int idx = 0; idx < sizeof(id_list_2horn)/sizeof(char);idx++){
      result = dxl_wb.ping(id_list_2horn[idx], &model_number, &log); //2축짜리 따로 핑
      if (result == false){
        Serial.println(log);
        Serial.println("Failed to ping");
      }
      else{
        Serial.println("Succeeded to ping");
        Serial.print("id : ");
        Serial.print(id_list_2horn[idx]);
        Serial.print(" model_number : ");
        Serial.println(model_number);
      }
  }
  
  bool result_move[sizeof(id_list)/sizeof(char)]; 
  for(int idx = 0 ; idx < sizeof(id_list)/sizeof(char) ; idx ++){
    result_move[idx]=dxl_wb.jointMode(id_list[idx], 0, 0, &log); //1축만 jointmode -> 각도제어
    if(result_move[idx]==false){
      Serial.println(log);
      Serial.print("Failed to change joint mode : ");
      Serial.println(idx);
    }
  }  
  Serial.println("Succeed to change joint mode");
  Serial.println("Dynamixel is moving...");
}
