#ifndef SHARE
#define SHARE
  
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
using namespace std;
using namespace cv;
  
//�޸Ĺ����ڴ�����
#define IMAGE_W 5472  //ͼ���
#define IMAGE_H 3648  //ͼ���
#define IMAGE_TYPE  CV_8UC3           // CV_8UC1 �Ҷ�ͼ   CV_8UC3 3ͨ��ͼ��
#define IMAGE_SIZE  IMAGE_W*IMAGE_H*3 //ͼƬ�����ܴ�С CV_8UC1--1ͨ��  CV_8UC3--3ͨ����ɫ
#define Shm_addrees 1209 //�����ڴ��ַ��ʶ
// ���еķ��غ��������з���ֵ ��Ȼ���ñ���
 
 
namespace MyShare{
  
  
    
  
 
  
     //�����ڴ�-ͼ��
        typedef struct ShareData
    {
        int  flag;
        int rows;//ͼ���
        int cols;//ͼ���
        char imgdata[IMAGE_SIZE];//ͼ������һά���ݣ�֮ǰ����cv::Mat���У���Ϊ�޷��ڽṹ�����ʼ����С
        float Gps[4];//����gps��Ϣ ��γ��ʱ���
    }ShareData_;
  
  
   // �ǹ����ڴ�-����gps�������
    typedef struct StructGpsData {
        int flag;
        char *msg;
        float longitude;
        float latitude;
        float high;
        float time;
    } ;
  
  
  
 class Share_class
{
    //��������
    public:
  
            //1���������ڴ�
            int shmid = shmget((key_t)Shm_addrees, sizeof(ShareData), 0666|IPC_CREAT);
            //2ӳ�乲���ڴ��ַ  shmָ���¼����ʼ��ַ
            void *shm = shmat(shmid, (void*)0, 0);//�������һ������ÿ�ε��ö�ִ�У���Ҫִ�����ͷ�һ��shmdt
            //printf("�����ڴ��ַ �� %p\n", (int *)(shm));
  
            //2-1��ShareData�ṹ������-���ʹ����ڴ�
            ShareData *pShareData= (ShareData*)shm;
  
            //��������ת����Ĺ����ڴ��ͼ����
            cv::Mat cvoutImg = cv::Mat(IMAGE_H,IMAGE_W,IMAGE_TYPE,cv::Scalar(255, 255, 255));//bufHight,bufWidth
               
           //δ������ ���Ը�һ��ͼ����� ���д�С3  ��ͣ��ͼ��Ȼ���Ŷ��������ڴ棬���˾������
             
    //��������
    public:
        //1��ʼ��ִ��
        Share_class(){
  
             printf("�����ڴ��ַ �� %p\n", (int *)(shm));
             //��ͼҪ�Ȱ�ͼ���־λ��ʼ��0��������ᵼ�½��ն˵��ÿ�����ʱ���ٴθ�0������1����ȡͼʧ�ܡ�
         }//1���캯��
           
         //0����ִ��
        ~Share_class() {
                cout<<"��������ִ��"<<endl;
                DestroyShare();
        }   //0�������ٺ���
  
  
        //3���ٹ����ڴ�
        int DestroyShare(){
  
            //4�Ͽ�ӳ�� ����֤�´η��ʲ���ռ��
            shmdt(shm);
            //5�ͷŹ����ڴ��ַ
            shmctl(shmid, IPC_RMID, 0);
            cout<<"�����ڴ��Ѿ�����"<<endl;
            return 1;
  
        }
        /*
        �������ܣ�   c++-��c++��-�������ڴ�
            c++ ģʽ����,
            c++ ����ͼ��һ�ŵ������ڴ� ��
            �޸�ͼ���־λ1������c++��pythoN���ý��պ�������ͼ��
        �������룺
            cv::Mat Img   Ҫ���͵�ͼ��
        ���������
            pShareData->flag = 1;   ͼ���־λ
            Mat cvoutImg                           ����ͼ�����
        */
      
        int Send_pic2_share_once(cv::Mat Img){
      
            int i = 0;
 
            if(pShareData->flag == 0)
            {
                                         
            //cv::Mat Img=cv::imread("../data_img/1.jpg",cv::IMREAD_COLOR);
 
                if(Img.data== nullptr)//nullptr��c++11�³��ֵĿ�ָ�볣��
                {
                    printf("ͼƬ�ļ�������\n");
                    return 0;
                    }
                else{
                    //   printf("ͼƬ�ļ�����\n");
                }
 
                //3-1�����ڴ汣��ͼ���͸�
                pShareData->rows =Img.rows;
                pShareData->cols =  Img.cols;
 
                //3-2�����ڴ汣��ͼ������
                int size = Img.cols * Img.rows * Img.channels();  
                char *cvoutImg = (char*)Img.data;
                memcpy(pShareData->imgdata, cvoutImg, size);
 
                //3-3�����ڴ汣���־λ        
                pShareData->flag = 1;
 
                return 1;
            }
            else{
 
                return 0;
            }
              
            //getchar();
        }
  
  
       /*
        �������ܣ�python �� c++-��c++��-�������ڴ�
            python �� c++ ���ɵ���
            c++ ��python ����ͼ��һ�ŵ������ڴ� ��
            �޸�ͼ���־λ1������c++��pythoN���ý��պ�������ͼ��
              
            �����pythonģʽͨ��c++����ã��ɿ���ͼ��չʾ��֤ͼ���Ƿ�����
        �������룺
            uchar *frame_data,      Ҫ����ͼ���ͼ������  Mat img.data
            int height,                         ͼ���
            int width                           ͼ���
        ���������
            pShareData->flag = 1;   ͼ���־λ
            Mat cvoutImg                    ����ͼ�����
        */
  
        int pySend_pic2_share_once(uchar *frame_data, int height, int width){
      
          if(pShareData->flag == 0)
            {
                //assert(height*width*3<=IMAGE_SIZE);
  
  
                    if(frame_data== nullptr)//nullptr��c++11�³��ֵĿ�ָ�볣��
                    {
                        printf("ͼƬ�ļ�������\n");
                        return 0;
                        }
                    else{
                       //   printf("ͼƬ�ļ�����\n");
                    }
                
                pShareData->cols=width;
                pShareData->rows=height;
                int size = height*width*3;
              
  
                memcpy(pShareData->imgdata, frame_data, size);
  
  
                //3-3�����ڴ汣���־λ        
                pShareData->flag = 1;
                //printf("���ݱ���ɹ� %d\n",pShareData->flag);
      
                /*
                //pythonģʽ��������֤���͵�ͼ���Ƿ������� python-��c++��-�������ڴ�
                int showimg=0; //���Ҫ��ʾ ������1
                if(!showimg) return 0;
  
                int channel=3;        
                cv::Mat image(height, width, CV_8UC3);
                for (int row = 0; row < height; row++) {
                    uchar *pxvec = image.ptr<uchar>(row);
                    for (int col = 0; col < width; col++) {
                        for (int c = 0; c < channel; c++) {
                            pxvec[col * channel + c] = frame_data[row * width * channel + channel * col + c];
                        }
                    }
                }
      
                cv::imshow("image", image);
                cv::waitKey(3);
             */
 
             return 1;
            }
            else{
 
              return 0;
            }
  
              
        }
  
  
  
       /*
        �������ܣ� �����ڴ� -> c++��-> c++
            C++ ����
            C++�ӹ����ڴ��ȡһ��ͼƬ
            �޸�ͼ���־λ0�������Ͷ��������ڴ��ͼ��
              
        �������룺
      
        ���������
            pShareData->flag = 0;   ͼ���־λ
            Mat cvoutImg                    ����ͼ�����
        */
  
  
        int  Rec_pic2_data_once()
        {  //cv::Mat &cvoutImg_in
            //3-1�����ڴ��ȡ��־λ    
            if(pShareData->flag == 1)
            {
                //3-2�ӹ����ڴ��ȡͼ��ߺͿ�
                int IMAGE_h=pShareData->rows;//�ӹ����ڴ��ȡͼ���
                int IMAGE_w=pShareData->cols;//�ӹ����ڴ��ȡͼ���
  
                //3-3�����ڴ��ȡͼ������
                //cv::Mat cvoutImg = cv::Mat(IMAGE_h,IMAGE_w,CV_8UC3,cv::Scalar(255, 255, 255));//bufHight,bufWidth
                int size = cvoutImg.cols * cvoutImg.rows * cvoutImg.channels();
                memcpy((char*)cvoutImg.data, pShareData->imgdata,size);
                //cv::imshow("RecData_Show",cvoutImg);
                //cv::waitKey(1);
            //  printf("���ݸ���һ��\n");
                //3-4�����ڴ��޸ı�־λ
                pShareData->flag = 0;
                return 1;
            }     
            else{
 
                return 0;
 
            }
        }
  
    
  
       /*
        �������ܣ� �����ڴ� -> c++��-> python �� c++
            python ���ã�C++��Rec_pic2_data_once��  ��Ҫ�Ǹ�python�û�ȡ�����ڴ��ͼ��
            python�ӹ����ڴ��ȡһ��ͼƬ
            �޸�ͼ���־λ0�������Ͷ��������ڴ��ͼ��
              
        �������룺
             ����ǰ����c++�˸��¹����ڴ����µ�ͼ��
      
        ���������
            pShareData->flag = 0;                     ͼ���־λ
            (uchar*)cvoutImg.data                  ����ͼ�����cvoutImg������dataָ��  
        */
  
uchar* Img_Cgg2py(){  //uchar* frame_data, int rows, int cols, int channels
                 
               
            //�������ڴ����е�ͼ�����ݷ���
                if(pShareData->flag == 1){
                    //  cvoutImg=cv::imread("/home/dongdong/3Code/python2c/1/c++2c++/v4_c++_class_python/img_data/00001.jpg");
                  
  
                    if(cvoutImg.data== nullptr)//nullptr��c++11�³��ֵĿ�ָ�볣��
                    {
                        printf("ͼƬ�ļ�������\n");
                        return 0;
                        }
                    else{
                       //   printf("ͼƬ�ļ�����\n");
                    }
  
                        //pShareData->flag = 0;  //��python�������ת����ͼ����python����ɱ�־λ0
                        return (uchar*)pShareData->imgdata;//����ֻ�Դ���һ�����飬˲�����
  
                       // pShareData->flag = 0;  //��python�������ת����ͼ����python����ɱ�־λ0
                }
                //���½����µ����ݷ���ģʽ
                /*
              
                Mat image =cv::imread("/home/dongdong/3Code/python2c/1/c++2c++/v4_c++_class_python/img_data/00001.jpg");
  
                
                if (!image.empty()) {
  
                //cout<<  "cgg2py new pic"<<endl;
                //cv::imshow("cgg", image);
                //cv::waitKey(0);
  
                int rows = image.rows;
                int cols = image.cols;
                int channels = image.channels();
                // printf("rows = %d cols = %d channels = %d size = %d\n", rows, cols, channels, rows*cols*channels);
                uchar *data = (uchar*)malloc(sizeof(uchar) * rows * cols * channels);
                memcpy(data, image.data, rows * cols * channels);
                return data;
                }
  
                */
}
  
  
       /*
        �������ܣ� �����ڴ� -> c++��-> python �� c++
            python ���ã�C++ֱ��ͨ����������ã�  ��Ҫ�Ǹ�pythonͨ��������ʽ�û�ȡ�����ڴ��int flag����
            ��ȡͼ���־λ,�����ж��Ƿ���Զ�д
          
        �������룺
              
        ���������
            pShareData->flag = 0 or 1;                     ͼ���־λ
            
        */
  
        //4-1��ȡͼ�񱣴��־λ
         int Get_ImgFlag(){
             return pShareData->flag ;
         }
  
        /*
        �������ܣ� �����ڴ� -> c++��-> python �� c++
            python ���ã�C++ֱ��ͨ����������ã�  ��Ҫ�Ǹ�pythonͨ��������ʽ���޸Ĺ����ڴ��int flag����
            ����ͼ���־λ,���ڿ����Ƿ���Զ�д
          
        �������룺
            int value                   pythoN��Ҫ������ת��Ϊ   ctypes.c_int  �ͽ��� Ĭ��int����Ҫ
        ���������
            pShareData->flag = 0 or 1;           ͼ���־λ    pythoN��Ҫ������ת��Ϊ   ctypes.c_int  ���� Ĭ��int����Ҫ
            
        */
  
         int Set_ImgFalg(int value){
            pShareData->flag =value;
         }
  
  
  
  
        /*
        �������ܣ�  python send -> c++�� ->  �����ڴ� -> c++��-> python rec
            python ���� ��c++��ֱ��ͨ����ı������þͿ��ԣ�
            python �޸Ĺ����ڴ��е�gps����
          
        �������룺
           float *data      python���������� �� ����
           int len               python�����鳤��
          
        ���������
            float result      ������� �ɲ�ʹ�ã�python���ն���Ҫָ��������������  c floatָ�� ctypes.POINTER(ctypes.c_float)  ��
            
        */
  
  
        //5��������  ����py���鲢�޸�pythonԭ����,�����ܺͽ��
        float pyarr_set_cgg_gps_share(float *data, int len) {
            float result=1;
            for (int i = 0; i < len; i++) {        
                     pShareData->Gps[i]=data[i] ;
                }
              return result;
                
            }
  
  
  
        /*
        �������ܣ�  python -> c++�� ->  �����ڴ� -> c++��-> python
            python ���� ��c++��ֱ��ͨ����ı������þͿ��ԣ�
            python ��ȡ�����ڴ��е�gps����
          
        �������룺
           float  pShareData->Gps[4]              C++ �����ڴ�ṹ��pShareData�е�GPS����
          
        ���������
            (uchar*) pShareData->Gps;       ��python���ն���Ҫָ��������������  c floatָ�� ctypes.POINTER(ctypes.c_float)  ��
            
        */
  
  
  
    uchar* py_get_cgg_gps_share(){
         // c++���Ͷ˵���������������GPS����,�����ڹ��ڴ棨�򵥾�����
         //pShareData->Gps[0]=1.56;
         //pShareData->Gps[1]=2.34;
         //pShareData->Gps[2]=3.14;
         //pShareData->Gps[3]=4.78;
  
         return (uchar*) pShareData->Gps; //����ָ��
    }
      
       
        /*
        �������ܣ�  python -> c++�� ->  �����ڴ� -> c++��-> python
            python ���� ��c++��ֱ��ͨ����ı������þͿ��ԣ�
            python ��ȡ�����ڴ��е�gps����  python���͹���һ���ṹ�壬�޸�pythoԭ�ṹ�巵��
          
        �������룺
           float  pShareData->Gps[4]              C++ �����ڴ�ṹ��pShareData�е�GPS����
           StructGpsData gps                             C++�Ľṹ��- python��Ҫ����Ӧ��python�ṹ������
          
        ���������
            StructGpsData gps       ��python���ն���Ҫָ��������������     ��
            
        */
  
  
  
    StructGpsData py_get_cgg_gps_Struct( StructGpsData gps){
       // c++���Ͷ˵���������������GPS����,�����ڹ��ڴ棨�򵥾�����
         //pShareData->Gps[0]=1.56;
         //pShareData->Gps[1]=2.34;
         //pShareData->Gps[2]=3.14;
         //pShareData->Gps[3]=4.78;
        
      //�����ڴ����ݸ���gps����
         gps.flag=1;
         gps.msg="new share data from c++  share";
         gps.longitude=pShareData->Gps[0];
         gps.latitude=pShareData->Gps[1];
         gps.high=pShareData->Gps[2];
         gps.time=pShareData->Gps[3];
         return gps;
    }
     
  
       
  
        /*
        �������ܣ�  python -> c++�� ->  �����ڴ�
            python ���� ��c++��ֱ��ͨ����ı������þͿ��ԣ�
            python �޸Ĺ����ڴ��е�gps����  python���͹���һ���ṹ�壬�޸�C++ԭ�ṹ��
          
        �������룺
           float  pShareData->Gps[4]              C++ �����ڴ�ṹ��pShareData�е�GPS����
           StructGpsData gps                             C++�Ľṹ��- python��Ҫ����Ӧ��python�ṹ������
          
        ���������
            StructGpsData gps       ��python���ն���Ҫָ��������������     ��
            
        */
  
    StructGpsData py_Set_cgg_gps_Struct( StructGpsData gps){
       // c++���Ͷ˵���������������GPS����,�����ڹ��ڴ棨�򵥾�����
         gps.flag=1;
         gps.msg="new share have set share c++";
         pShareData->Gps[0]=gps.longitude;
         pShareData->Gps[1]=gps.latitude;
         pShareData->Gps[2]= gps.high;
         pShareData->Gps[3]=gps.time;
        
      
  
         return gps;
    }
  
  
  
  
     };//�ඨ�����
  
  
}//namespace ����
  
  
//����C���Ը�ʽ���´��-python����
//������ö˿������߳�  �ⲿ���޷�ֱ�ӷ��ʵ� ��Ҫ���߳��������´��������
//����ǵ�����C++���ã�����Ҫ������ӷ�װ
  
extern "C" {
  
    MyShare::Share_class useShare;
  
    int DestroyShare_(){
         useShare.DestroyShare();
    }
  
  
    int Send_pic2_share_once_(cv::Mat Img){
  
         useShare.Send_pic2_share_once(Img);
  
    }
  
    int pySend_pic2_share_once_(uchar *frame_data, int height, int width){
         useShare.pySend_pic2_share_once(frame_data,  height, width);
    }
  
  
  
    int  Rec_pic2_data_once_(){
  
         useShare.Rec_pic2_data_once();
  
    }
  
    uchar* Img_Cgg2py_(){
         useShare.Img_Cgg2py();
    }
  
  
    int Get_ImgFlag_(){
            useShare.Get_ImgFlag();
    }
  
  
    int Set_ImgFalg_(int value){
  
        useShare.Set_ImgFalg(value);
    }
  
          
   float pyarr_set_cgg_gps_share_(float *data, int len) {
        useShare.pyarr_set_cgg_gps_share( data,  len);
    }
  
  
    uchar*  py_get_cgg_gps_share_(){
                useShare.py_get_cgg_gps_share();
      }
  
  
  
   MyShare::StructGpsData py_get_cgg_gps_Struct_( MyShare::StructGpsData gps){
             return useShare.py_get_cgg_gps_Struct(gps);
    }
  
  
    MyShare::StructGpsData py_Set_cgg_gps_Struct_(MyShare::StructGpsData gps){
            return useShare.py_Set_cgg_gps_Struct(gps);
    }
  
  
  
}
  
  
  
  
  
  
  
  
#endif