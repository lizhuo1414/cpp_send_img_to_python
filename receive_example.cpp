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
    //1��������
    //1-1������ö����µ��̵߳����ˣ���Ҫ���µ��̴߳��������
    //1-2�������python����� ����ע����C��ʽ�����װ ÿ�ε�����ʱ����
    //MyShare::Share_class useShare;
 
    //2��������ͼ��
     cv::Mat RecImg = cv::Mat(IMAGE_H,IMAGE_W,IMAGE_TYPE,cv::Scalar(255, 255, 255));//bufHight,bufWidth
     cv::namedWindow("RecData_Show",0);
 
    //3����GPS����
    float Gps_c[4];
  
    //3����ͼ��
    while (1){
  
            sleep(0.001);//��ֹ���ԵĿ�ѭ������CPuռ�ã����������޷��޸Ĺ����ڴ�
             
            //����GPS
            if( useShare.pShareData->flag ==1){
                  
                Gps_c[0]=useShare.pShareData->Gps[0];
                Gps_c[1]=useShare.pShareData->Gps[1];
                Gps_c[2]=useShare.pShareData->Gps[2];
                Gps_c[3]=useShare.pShareData->Gps[3];
  
                for(int i=0;i<=3;i++){cout<<  "GPS_DATA"<<i <<"    "<<Gps_c[i]  <<"    ";}
                cout<<endl;
  
            }
  
            //����ͼ��
            if( useShare.pShareData->flag ==1){//�洢��ϣ������ͼ
                useShare.Rec_pic2_data_once();//����һ��ͼ���Զ�������useShare.cvoutImg�У�Ȼ���޸ı�־λ
                RecImg=useShare.cvoutImg;
                useShare.pShareData->flag =0;//��ȡ��ϣ������ͼ
  
            }
                cv::imshow("RecData_Show",RecImg);
                char key=cv::waitKey(1);
                if(key=='q'){      useShare.pShareData->flag=3;     printf("�����˳�");   break;}  
  
    }
  
    //����
    //useShare.DestroyShare(); //���ٹ����ڴ� ���ͷ��Զ�����
  
  
    return 0;
}