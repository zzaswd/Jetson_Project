//======================================
// Topic Subscribe
// angle과 mode Topic subscribe.
// mode에 따라 Main문에서 동작 다르게 수행.
//=====================================

//int mode = -1;
int receiveData;
//int flag = 0;
//int movingOK = 0;

void modeCb( const std_msgs::String& msg) {
  std::stringstream stream;
  stream.str(msg.data);
  //    echo_msg.data = msg.data;
  stream >> receiveData;
  stream.str("");
  if (receiveData == 1) mode = 1; // init_셋팅 후 Dynamixel pub;
  else if (receiveData == 3) mode = 3; // del_X sub해서 파싱.
  else if (receiveData == 4) mode = 4; // del_Y sub해서 파싱.
  else if (receiveData == 6) mode = 6; // del_Y sub해서 파싱.
}

void angleCb( const std_msgs::String& msg) {
  std::stringstream stream;
  stream.str(msg.data);

  getline(stream, angle[0], '@');
  getline(stream, angle[1], 'L');
  
  flag = 1;
  stream.str("");
}
