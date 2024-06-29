#!/usr/bin/python
# coding=utf-8
import numpy as np
import time
import ctypes
import os
import cv2
 
 
#更多类型参看  https://docs.python.org/3/library/ctypes.html  
# c_bool  c_char    c_int    c_long  c_float   c_double 
# c_char_p  c_wchar_p   c_void_p
 
#1载入库
libLoad = ctypes.cdll.LoadLibrary
sharelib = libLoad("./build/libpython2share.so")
 
 
 
#--------------0-发送单个int   初始第一次设置为0 允许开始存图------------------------
sharelib.Set_ImgFalg_.argtype = ctypes.c_int # 设置函数输入值类型 int可忽略
#sharelib.Set_ImgFalg_(0) #修改c++类的某一个变量（py只能通过函数而非直接访问修改）
sharelib.Get_ImgFlag_.restype = ctypes.c_int # 设置返回值类型 int可忽略
#reuslt=sharelib.Get_ImgFlag_()#获取变量的结果
 
 
#--------------------2-1 数组修改GPS ----------------
 
def py_Arr_GPS(pyarray):
 
    sharelib.pyarr_set_cgg_gps_share_.restype = ctypes.c_float #定义函数返回值类型
 
    #pyarray = [1., 2., 3., 4.]                             #创建py数组
    carray2cgg = (ctypes.c_float * len(pyarray))(*pyarray) #转化成c_float指针
    reuls_sum=sharelib.pyarr_set_cgg_gps_share_(carray2cgg, len(pyarray)) #调用查询
 
    size=(1,4) #将获取的数组变成 1*3矩阵
    pose = np.reshape(np.array(np.fromiter(carray2cgg, np.float64, size[0]*size[1])), size)
    print("c++修改后返回，python n*n维度数组:\n",pose)
 
 
#--------------------2-2 结构体修改GPS ----------------
#python创建结构体
class py_Struct_GPS(ctypes.Structure):
    _fields_ = [
        ('flag', ctypes.c_int),   #c++ int
        ('msg', ctypes.c_char_p), #c++   char*
        ('longitude', ctypes.c_float), # c++ float
        ('latitude', ctypes.c_float),# c++ float
        ('high', ctypes.c_float),# c++ float
        ('time', ctypes.c_float)# c++ float
        ]    #c++ char*[]
#python结构体赋予初值
struct_gps=py_Struct_GPS()
struct_gps.flag=ctypes.c_int(0)
struct_gps.msg=ctypes.c_char_p('GPS DATA'.encode())
struct_gps.longitude=ctypes.c_float(0.)
struct_gps.latitude=ctypes.c_float(0.)
struct_gps.high=ctypes.c_float(0.)
struct_gps.time=ctypes.c_float(0.)
 
print("结构体gps_old \n",struct_gps)
print(struct_gps.flag,struct_gps.msg.decode(),struct_gps.longitude,struct_gps.latitude,struct_gps.high,struct_gps.time)
#0 GPS DATA 0.0 0.0 0.0 0.0
 
 
def SetGPSToShare_struct(struct_gps,flag,msg,longitude,latitude,high,time):
 
    struct_gps.flag=ctypes.c_int(flag)
    struct_gps.msg=ctypes.c_char_p(msg.encode())
    struct_gps.longitude=ctypes.c_float(longitude)
    struct_gps.latitude=ctypes.c_float(latitude)
    struct_gps.high=ctypes.c_float(high)
    struct_gps.time=ctypes.c_float(time)
 
    #从c++获取数据-数据保存在 共享内存或C++直接生成
    # 定义返回类型为结构体类型
    sharelib.py_Set_cgg_gps_Struct_.restype = py_Struct_GPS
    # 调用获取gps数据-返回数据结构体
    struct_gps= sharelib.py_Set_cgg_gps_Struct_(struct_gps)
    #print(struct_gps.flag)
    print(struct_gps.msg.decode())  #必须解码
    #print("结构体gps_new \n",struct_gps)
    #print(struct_gps.flag,struct_gps.msg.decode(),struct_gps.longitude,struct_gps.latitude,struct_gps.high,struct_gps.time)
    #1 new data 1.559999942779541 2.559999942779541 3.559999942779541 5.559999942779541
 
#--------------------2-2 结构体拿GPS ----------------
 
 
 
 
#3发送图像
 
sharelib.Set_ImgFalg_(0) #允许存图
pyimage = cv2.imread("img_data/00001.jpg")#读取原始图像
#cv2.imshow("1 ", pyimage)
#cv2.waitKey(0)
 
def SendImgFromShare(image):
    image_data = np.asarray(image, dtype=np.uint8)#图像转化成数组
    image_data = image_data.ctypes.data_as(ctypes.c_char_p)#数组转化成C指针一维数组
    sharelib.pySend_pic2_share_once_(image_data,image.shape[0],image.shape[1])#发送到共享内存
 
 
 
while True:
 
    time.sleep(0.001)#防止绝对的空循环导致CPu占用，其他进程无法修改共享内存
 
   #发送gps数据-数组模式
    gps_p=[1.31, 3.42, 2.41, 6.41]
    py_Arr_GPS(gps_p)
 
    #发送gps数据-结构体模式
    #SetGPSToShare_struct(struct_gps,0,"set gps to share",11.034,145.565,80.0,121314)
 
 
     #发送图像数据
    if sharelib.Get_ImgFlag_()==0:#读图结束，允许存图
        SendImgFromShare(pyimage)#发送图像
        sharelib.Set_ImgFalg_(1)#存图结束，允许读图
 
 
    if sharelib.Get_ImgFlag_()==3:
        print("接收端停止")
        break