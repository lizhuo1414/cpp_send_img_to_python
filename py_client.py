#!/usr/bin/python
# coding=gbk
import ctypes
import time
from ctypes import *
import os
import cv2
import numpy as np
from datetime import datetime
# �������Ͳο�  https://docs.python.org/3/library/ctypes.html
# c_bool  c_char    c_int    c_long  c_float   c_double
# c_char_p  c_wchar_p   c_void_p

# 1�����
libLoad = ctypes.cdll.LoadLibrary
sharelib = libLoad("./libpython2share.so")


# --------------0-���͵���int   ��ʼ��һ������Ϊ0 ����ʼ��ͼ------------------------
sharelib.Set_ImgFalg_.argtype = ctypes.c_int  # ���ó�ʼֵ����
# sharelib.Set_ImgFalg_(0) #�޸�c++���ĳһ��������pyֻ��ͨ����������ֱ�ӷ����޸ģ�
sharelib.Get_ImgFlag_.restype = ctypes.c_int  # ���÷���ֵ����
# reuslt=sharelib.Get_ImgFlag_()#��ȡ�����Ľ��


# -----------------2-1 ��ȡgps����----------------
# ���������������Ϊuint8��ָ��

def ReadGPSFromShare():
    sharelib.py_get_cgg_gps_share_.restype = ctypes.POINTER(
        ctypes.c_float)  # c_char_p
    # ��ȡ���ͼ������ָ��
    pointer_f_gps = sharelib.py_get_cgg_gps_share_()
    size = (1, 4)
    # ����ת����ͼ��
    gps = np.reshape(np.array(np.fromiter(
        pointer_f_gps, dtype=np.float, count=size[0]*size[1])), size)
    print("����ģʽ��ȡ��GPS:", gps)
    return gps
# -----------------2-1 ��ȡgps�������----------------

# --------------------2-2 �ṹ����GPS ----------------
# python�����ṹ��


class py_Struct_GPS(ctypes.Structure):
    _fields_ = [
        ('flag', ctypes.c_int),  # c++ int
        ('msg', ctypes.c_char_p),  # c++   char*
        ('longitude', ctypes.c_float),  # c++ float
        ('latitude', ctypes.c_float),  # c++ float
        ('high', ctypes.c_float),  # c++ float
        ('time', ctypes.c_float)  # c++ float
    ]  # c++ char*[]


# python�ṹ�帳���ֵ
struct_gps = py_Struct_GPS()
struct_gps.flag = ctypes.c_int(0)
struct_gps.msg = ctypes.c_char_p('GPS DATA'.encode())
struct_gps.longitude = ctypes.c_float(0.)
struct_gps.latitude = ctypes.c_float(0.)
struct_gps.high = ctypes.c_float(0.)
struct_gps.time = ctypes.c_float(0.)

# print("�ṹ��gps_old \n", struct_gps)
# print(struct_gps.flag, struct_gps.msg.decode(), struct_gps.longitude,
#       struct_gps.latitude, struct_gps.high, struct_gps.time)
# 0 GPS DATA 0.0 0.0 0.0 0.0

# ��c++��ȡ����-���ݱ����� �����ڴ��C++ֱ������
# ���巵������Ϊ�ṹ������
sharelib.py_get_cgg_gps_Struct_.restype = py_Struct_GPS
# ���û�ȡgps����-�������ݽṹ��
struct_gps = sharelib.py_get_cgg_gps_Struct_(struct_gps)
# print(struct_gps.flag)
# print(struct_gps.msg.decode())  #�������
# print("�ṹ��gps_new \n", struct_gps)
# print(struct_gps.flag, struct_gps.msg.decode(), struct_gps.longitude,
#       struct_gps.latitude, struct_gps.high, struct_gps.time)
# 1 new data 1.559999942779541 2.559999942779541 3.559999942779541 5.559999942779541

# --------------------2-2 �ṹ����GPS ----------------


# -----------------3python read cgg ��ȡͼ��-------------------

def ReadImgFromShare():
    start_time = time.time()
    # ���������������Ϊuint8��ָ��
    sharelib.Img_Cgg2py_.restype = ctypes.POINTER(ctypes.c_uint8)  # c_char_p
    # ��ȡ���ͼ������ָ��
    pointer_img = sharelib.Img_Cgg2py_()
    size = (720, 1280, 3)
    # ����ת����ͼ��
    date_type = c_uint8*(size[0]*size[1]*size[2])
    # RecImg = np.reshape(np.array(np.fromiter(
    #     pointer_img, dtype=np.uint8, count=size[0]*size[1]*size[2])), size)
    data = ctypes.cast(pointer_img, ctypes.POINTER(date_type)).contents
    RecImg = np.frombuffer(data,dtype=np.uint8,count=-1).reshape(size)
    end_time = time.time()
    execution_time = end_time - start_time
    print("get image!:", execution_time)
    # cv2.imwrite("./1.jpg", RecImg)
    # cv2.imshow("showimg ", RecImg)
    # cv2.waitKey(1)

    '''
    print(RecImg)
    if RecImg is not None:
        #չʾͼ��
        #cv2.namedWindow("showimg",0)
        cv2.imshow("showimg ", RecImg)
        cv2.waitKey(1)
        #return RecImg
    else:
        return 1
    '''
    return RecImg


while True:

    time.sleep(0.001)  # ��ֹ���ԵĿ�ѭ������CPuռ�ã����������޷��޸Ĺ����ڴ�

    if sharelib.Get_ImgFlag_() == 1:  # ��ͼ�����������ͼ

        # ����1-ֱ�ӷ��ʹ����ڴ����GPS����
        # PyArr1x4Gps=ReadGPSFromShare()#��ȡGPS
        # print("pythonֱ�ӷ��ʹ����ڴ�GPS��������:\n",PyArr1x4Gps[0][0],PyArr1x4Gps[0][1],PyArr1x4Gps[0][2],PyArr1x4Gps[0][3])

        # ����2-ͨ���ṹ����ʹ����ڴ����GPS����
        # ���û�ȡgps����-�������ݽṹ��
        # struct_gps= sharelib.py_get_cgg_gps_Struct_(struct_gps)
        # print("pythonͨ���ṹ���ȡ�����ڴ�GPS���ݣ�\n",struct_gps.flag,struct_gps.msg.decode(),struct_gps.longitude,struct_gps.latitude,struct_gps.high,struct_gps.time)

       # ����ͼ��
        Py_img = ReadImgFromShare()

        # չʾͼ��
        # cv2.namedWindow("showimg1",0)
        # cv2.imshow("showimg1 ", Py_img)

        # cv2.waitKey(1)

        sharelib.Set_ImgFalg_(0)  # ��ͼ�����������ͼ

    if sharelib.Get_ImgFlag_() == 3:
        break

