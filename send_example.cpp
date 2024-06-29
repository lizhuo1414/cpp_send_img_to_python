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
  
    //1创建对象  可以不创建 调用文件自动有一个
    //1-1如果调用端在新的线程调用了，需要在新的线程创建这个类
    //1-2如果不用python库编译 可以注销了C格式的类封装 每次调用临时创建
    //MyShare::Share_class useShare; //头文件c++封装c函数时候已经创建了
      
    //2-1发送数据-读取数据
    cv::Mat Img=cv::imread("1280.jpg",cv::IMREAD_COLOR);
        
    //cv::imshow("RecData_Show",Img);
    //char key=cv::waitKey(0);
 
    //2-2修改标志位--用来控制读取同步
    useShare.pShareData->flag = 0;  //提醒。第一次初始化强制给0，清除上次程序残留结果,类初始化已经给0了
      
      
    //3发送图像
   while (1){
        
      sleep(0.001);//防止绝对的空循环导致CPu占用，其他进程无法修改共享内存
  
      //发送gps-C++直接访问类变量发送修改
       if(  useShare.pShareData->flag ==0){
       useShare.pShareData->Gps[0]=32.3;
       useShare.pShareData->Gps[1]=12.3;
       useShare.pShareData->Gps[2]=72.3;
       useShare.pShareData->Gps[3]=12.3;
       }
  
        
  
        if( useShare.pShareData->flag ==0){//读取完毕，允许存图
        // 发送图像-C++模式 T256相机通用模式 输入Mat 类型图像
        //useShare.Send_pic2_share_once(Img);//发送一张图像
  
        //发送图像-C++和python模式 注意图像格式为 uchar* 数组 有的数据例如 T265图像就用不了
        useShare.pySend_pic2_share_once((uchar*)Img.data,Img.rows,Img.cols);//发送一张图
        useShare.pShareData->flag =1;//存储完毕，允许读图
        }
  
  
  
        if(useShare.pShareData->flag ==3){ //接收端读取成功并且将标志位修改成3 关闭
            break;
        }
   }
   //销毁
    //useShare.DestroyShare(); //销毁共享内存  类释放时候会自动销毁，这里做个提醒
  
}