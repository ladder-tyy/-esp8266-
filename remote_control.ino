#define BLINKER_WIFI
#define BLINKER_MIOT_OUTLET   //小爱同学配置电源类设备的包

#include <Blinker.h>
#include <ESP8266WiFi.h>

char auth[] = "*********";//密钥
char ssid[] = "*********";//常用wifi名字，也可以用微信扫pcb后面的二维码进行配网
char pswd[] = "*********";//wifi密码

// 新建组件对象
BlinkerButton Button1("********");//添加一个按钮就够了，把名字替换前面星号
int GPIO = 16;
//定义使用函数
void SmartConfig();
bool AutoConfig();
void button1_callback(const String & state);
void miotQuery(int32_t queryCode);
void dataRead(const String & data);
void miotPowerState(const String & state);

void setup(){
    // 初始化串口
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);
    BLINKER_DEBUG.debugAll();
    
    // 初始化有LED的IO
    pinMode(GPIO, OUTPUT);
    
    //自动联网
    if (!AutoConfig()){
        SmartConfig();
    }
    
    // 初始化blinker
    Blinker.begin(auth, ssid, pswd);

    Button1.attach(button1_callback);
     //blinker数据接收
     Blinker.attachData(dataRead);
     //小爱同学反馈信息，调用设备查询函数，查询设备开关机状态
     BlinkerMIOT.attachQuery(miotQuery);  
     //小爱同学反馈信息，小爱控制开关机操作
     BlinkerMIOT.attachPowerState(miotPowerState);
}

void loop(){
    Blinker.run();
}

//自动连接上一次wifi
void SmartConfig(){
  WiFi.mode(WIFI_STA);
  Serial.println("\r\nWait for Smartconfig...");
  WiFi.beginSmartConfig();
  while (1)
  {
    Serial.print(".");
    delay(500);                   // wait for a second
    if (WiFi.smartConfigDone())
    {
      Serial.println("SmartConfig Success");
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      break;
    }
  }
}

bool AutoConfig(){
    WiFi.begin();
    //如果觉得时间太长可改
    for (int i = 0; i < 20; i++)
    {
        int wstatus = WiFi.status();
        if (wstatus == WL_CONNECTED){
            Serial.println("WIFI SmartConfig Success");
            Serial.printf("SSID:%s", WiFi.SSID().c_str());
            Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
            Serial.print("LocalIP:");
            Serial.print(WiFi.localIP());
            Serial.print(" ,GateIP:");
            Serial.println(WiFi.gatewayIP());
            return true;
        }
        else{
            Serial.print("WIFI AutoConfig Waiting......");
            Serial.println(wstatus);
            delay(1000);
        }
    }
    Serial.println("WIFI AutoConfig Faild!" );
    return false;
}

//按钮回调函数
void button1_callback(const String & state){
  digitalWrite(GPIO,!digitalRead(GPIO));
}

//自定义小爱同学状态查询的回调函数
void miotQuery(int32_t queryCode){
  BLINKER_LOG("XiaoAi Query codes: ", queryCode);
  switch (queryCode){
    //小爱查询全部内容
    case BLINKER_CMD_QUERY_ALL_NUMBER :
      BLINKER_LOG("MIOT Query All");
      BlinkerMIOT.powerState(digitalRead(GPIO) ? "off" : "on");
      BlinkerMIOT.print();
      break;
    //小爱同学查询插座状态
    case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
      BLINKER_LOG("MIOT Query Power State");
      BlinkerMIOT.powerState(digitalRead(GPIO) ? "off" : "on");
      BlinkerMIOT.print();
      break;
    default :
      BlinkerMIOT.powerState(digitalRead(GPIO) ? "off" : "on");
      BlinkerMIOT.print();
      break;
  }
}

//如果未绑定的组件被触发,则会执行其中内容
void dataRead(const String & data){
  //读取字符串，打印
  BLINKER_LOG("Blinker ReadString: ", data);
  //起刷新作用
  Blinker.vibrate();
  //获取到系统运行的时间
  uint32_t BlinkerTime = millis();
  //打印出来
  Blinker.print("millis", BlinkerTime);
}


//自定义小爱同学电源类操作的回调函数
void miotPowerState(const String & state){    
  BLINKER_LOG("XiaoAi Set Power State: ", state);
  //开启
  if (state == BLINKER_CMD_ON){  
    digitalWrite(GPIO, HIGH);
    //更新状态
    BlinkerMIOT.powerState("on");
    BlinkerMIOT.print();
  // 关闭
  }else if (state == BLINKER_CMD_OFF){ 
    digitalWrite(GPIO, LOW);
    //更新状态
    BlinkerMIOT.powerState("off");
    BlinkerMIOT.print();
  }
}
