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
  
    //1��������  ���Բ����� �����ļ��Զ���һ��
    //1-1������ö����µ��̵߳����ˣ���Ҫ���µ��̴߳��������
    //1-2�������python����� ����ע����C��ʽ�����װ ÿ�ε�����ʱ����
    //MyShare::Share_class useShare; //ͷ�ļ�c++��װc����ʱ���Ѿ�������
      
    //2-1��������-��ȡ����
    cv::Mat Img=cv::imread("1280.jpg",cv::IMREAD_COLOR);
        
    //cv::imshow("RecData_Show",Img);
    //char key=cv::waitKey(0);
 
    //2-2�޸ı�־λ--�������ƶ�ȡͬ��
    useShare.pShareData->flag = 0;  //���ѡ���һ�γ�ʼ��ǿ�Ƹ�0������ϴγ���������,���ʼ���Ѿ���0��
      
      
    //3����ͼ��
   while (1){
        
      sleep(0.001);//��ֹ���ԵĿ�ѭ������CPuռ�ã����������޷��޸Ĺ����ڴ�
  
      //����gps-C++ֱ�ӷ�������������޸�
       if(  useShare.pShareData->flag ==0){
       useShare.pShareData->Gps[0]=32.3;
       useShare.pShareData->Gps[1]=12.3;
       useShare.pShareData->Gps[2]=72.3;
       useShare.pShareData->Gps[3]=12.3;
       }
  
        
  
        if( useShare.pShareData->flag ==0){//��ȡ��ϣ������ͼ
        // ����ͼ��-C++ģʽ T256���ͨ��ģʽ ����Mat ����ͼ��
        //useShare.Send_pic2_share_once(Img);//����һ��ͼ��
  
        //����ͼ��-C++��pythonģʽ ע��ͼ���ʽΪ uchar* ���� �е��������� T265ͼ����ò���
        useShare.pySend_pic2_share_once((uchar*)Img.data,Img.rows,Img.cols);//����һ��ͼ
        useShare.pShareData->flag =1;//�洢��ϣ������ͼ
        }
  
  
  
        if(useShare.pShareData->flag ==3){ //���ն˶�ȡ�ɹ����ҽ���־λ�޸ĳ�3 �ر�
            break;
        }
   }
   //����
    //useShare.DestroyShare(); //���ٹ����ڴ�  ���ͷ�ʱ����Զ����٣�������������
  
}