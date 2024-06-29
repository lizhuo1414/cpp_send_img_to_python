#!/usr/bin/python
# coding=utf-8
import numpy as np
import time
import ctypes
import os
import cv2
 
 
#�������Ͳο�  https://docs.python.org/3/library/ctypes.html  
# c_bool  c_char    c_int    c_long  c_float   c_double 
# c_char_p  c_wchar_p   c_void_p
 
#1�����
libLoad = ctypes.cdll.LoadLibrary
sharelib = libLoad("./build/libpython2share.so")
 
 
 
#--------------0-���͵���int   ��ʼ��һ������Ϊ0 ����ʼ��ͼ------------------------
sharelib.Set_ImgFalg_.argtype = ctypes.c_int # ���ú�������ֵ���� int�ɺ���
#sharelib.Set_ImgFalg_(0) #�޸�c++���ĳһ��������pyֻ��ͨ����������ֱ�ӷ����޸ģ�
sharelib.Get_ImgFlag_.restype = ctypes.c_int # ���÷���ֵ���� int�ɺ���
#reuslt=sharelib.Get_ImgFlag_()#��ȡ�����Ľ��
 
 
#--------------------2-1 �����޸�GPS ----------------
 
def py_Arr_GPS(pyarray):
 
    sharelib.pyarr_set_cgg_gps_share_.restype = ctypes.c_float #���庯������ֵ����
 
    #pyarray = [1., 2., 3., 4.]                             #����py����
    carray2cgg = (ctypes.c_float * len(pyarray))(*pyarray) #ת����c_floatָ��
    reuls_sum=sharelib.pyarr_set_cgg_gps_share_(carray2cgg, len(pyarray)) #���ò�ѯ
 
    size=(1,4) #����ȡ�������� 1*3����
    pose = np.reshape(np.array(np.fromiter(carray2cgg, np.float64, size[0]*size[1])), size)
    print("c++�޸ĺ󷵻أ�python n*nά������:\n",pose)
 
 
#--------------------2-2 �ṹ���޸�GPS ----------------
#python�����ṹ��
class py_Struct_GPS(ctypes.Structure):
    _fields_ = [
        ('flag', ctypes.c_int),   #c++ int
        ('msg', ctypes.c_char_p), #c++   char*
        ('longitude', ctypes.c_float), # c++ float
        ('latitude', ctypes.c_float),# c++ float
        ('high', ctypes.c_float),# c++ float
        ('time', ctypes.c_float)# c++ float
        ]    #c++ char*[]
#python�ṹ�帳���ֵ
struct_gps=py_Struct_GPS()
struct_gps.flag=ctypes.c_int(0)
struct_gps.msg=ctypes.c_char_p('GPS DATA'.encode())
struct_gps.longitude=ctypes.c_float(0.)
struct_gps.latitude=ctypes.c_float(0.)
struct_gps.high=ctypes.c_float(0.)
struct_gps.time=ctypes.c_float(0.)
 
print("�ṹ��gps_old \n",struct_gps)
print(struct_gps.flag,struct_gps.msg.decode(),struct_gps.longitude,struct_gps.latitude,struct_gps.high,struct_gps.time)
#0 GPS DATA 0.0 0.0 0.0 0.0
 
 
def SetGPSToShare_struct(struct_gps,flag,msg,longitude,latitude,high,time):
 
    struct_gps.flag=ctypes.c_int(flag)
    struct_gps.msg=ctypes.c_char_p(msg.encode())
    struct_gps.longitude=ctypes.c_float(longitude)
    struct_gps.latitude=ctypes.c_float(latitude)
    struct_gps.high=ctypes.c_float(high)
    struct_gps.time=ctypes.c_float(time)
 
    #��c++��ȡ����-���ݱ����� �����ڴ��C++ֱ������
    # ���巵������Ϊ�ṹ������
    sharelib.py_Set_cgg_gps_Struct_.restype = py_Struct_GPS
    # ���û�ȡgps����-�������ݽṹ��
    struct_gps= sharelib.py_Set_cgg_gps_Struct_(struct_gps)
    #print(struct_gps.flag)
    print(struct_gps.msg.decode())  #�������
    #print("�ṹ��gps_new \n",struct_gps)
    #print(struct_gps.flag,struct_gps.msg.decode(),struct_gps.longitude,struct_gps.latitude,struct_gps.high,struct_gps.time)
    #1 new data 1.559999942779541 2.559999942779541 3.559999942779541 5.559999942779541
 
#--------------------2-2 �ṹ����GPS ----------------
 
 
 
 
#3����ͼ��
 
sharelib.Set_ImgFalg_(0) #�����ͼ
pyimage = cv2.imread("img_data/00001.jpg")#��ȡԭʼͼ��
#cv2.imshow("1 ", pyimage)
#cv2.waitKey(0)
 
def SendImgFromShare(image):
    image_data = np.asarray(image, dtype=np.uint8)#ͼ��ת��������
    image_data = image_data.ctypes.data_as(ctypes.c_char_p)#����ת����Cָ��һά����
    sharelib.pySend_pic2_share_once_(image_data,image.shape[0],image.shape[1])#���͵������ڴ�
 
 
 
while True:
 
    time.sleep(0.001)#��ֹ���ԵĿ�ѭ������CPuռ�ã����������޷��޸Ĺ����ڴ�
 
   #����gps����-����ģʽ
    gps_p=[1.31, 3.42, 2.41, 6.41]
    py_Arr_GPS(gps_p)
 
    #����gps����-�ṹ��ģʽ
    #SetGPSToShare_struct(struct_gps,0,"set gps to share",11.034,145.565,80.0,121314)
 
 
     #����ͼ������
    if sharelib.Get_ImgFlag_()==0:#��ͼ�����������ͼ
        SendImgFromShare(pyimage)#����ͼ��
        sharelib.Set_ImgFalg_(1)#��ͼ�����������ͼ
 
 
    if sharelib.Get_ImgFlag_()==3:
        print("���ն�ֹͣ")
        break