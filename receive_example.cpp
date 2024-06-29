#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <vector>
#include <string>
  
#include  "ApiShare_dll.cpp"
using namespace std;
   
  
int main()
{
    //1创建对象
    //1-1如果调用端在新的线程调用了，需要在新的线程创建这个类
    //1-2如果不用python库编译 可以注销了C格式的类封装 每次调用临时创建
    //MyShare::Share_class useShare;
 
    //2创建接受图像
     cv::Mat RecImg = cv::Mat(IMAGE_H,IMAGE_W,IMAGE_TYPE,cv::Scalar(255, 255, 255));//bufHight,bufWidth
     cv::namedWindow("RecData_Show",0);
 
    //3接受GPS数据
    float Gps_c[4];
  
    //3接受图像
    while (1){
  
            sleep(0.001);//防止绝对的空循环导致CPu占用，其他进程无法修改共享内存
             
            //接受GPS
            if( useShare.pShareData->flag ==1){
                  
                Gps_c[0]=useShare.pShareData->Gps[0];
                Gps_c[1]=useShare.pShareData->Gps[1];
                Gps_c[2]=useShare.pShareData->Gps[2];
                Gps_c[3]=useShare.pShareData->Gps[3];
  
                for(int i=0;i<=3;i++){cout<<  "GPS_DATA"<<i <<"    "<<Gps_c[i]  <<"    ";}
                cout<<endl;
  
            }
  
            //接受图像
            if( useShare.pShareData->flag ==1){//存储完毕，允许读图
                useShare.Rec_pic2_data_once();//接受一张图像自动保存在useShare.cvoutImg中，然后修改标志位
                RecImg=useShare.cvoutImg;
                useShare.pShareData->flag =0;//读取完毕，允许存图
  
            }
                cv::imshow("RecData_Show",RecImg);
                char key=cv::waitKey(1);
                if(key=='q'){      useShare.pShareData->flag=3;     printf("按键退出");   break;}  
  
    }
  
    //销毁
    //useShare.DestroyShare(); //销毁共享内存 类释放自动销毁
  
  
    return 0;
}