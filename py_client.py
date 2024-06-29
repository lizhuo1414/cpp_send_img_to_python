#!/usr/bin/python
# coding=gbk
import ctypes
import time
from ctypes import *
import os
import cv2
import numpy as np
from datetime import datetime
# 更多类型参看  https://docs.python.org/3/library/ctypes.html
# c_bool  c_char    c_int    c_long  c_float   c_double
# c_char_p  c_wchar_p   c_void_p

# 1载入库
libLoad = ctypes.cdll.LoadLibrary
sharelib = libLoad("./libpython2share.so")


# --------------0-发送单个int   初始第一次设置为0 允许开始存图------------------------
sharelib.Set_ImgFalg_.argtype = ctypes.c_int  # 设置初始值类型
# sharelib.Set_ImgFalg_(0) #修改c++类的某一个变量（py只能通过函数而非直接访问修改）
sharelib.Get_ImgFlag_.restype = ctypes.c_int  # 设置返回值类型
# reuslt=sharelib.Get_ImgFlag_()#获取变量的结果


# -----------------2-1 获取gps数组----------------
# 设置输出数据类型为uint8的指针

def ReadGPSFromShare():
    sharelib.py_get_cgg_gps_share_.restype = ctypes.POINTER(
        ctypes.c_float)  # c_char_p
    # 获取输出图像数据指针
    pointer_f_gps = sharelib.py_get_cgg_gps_share_()
    size = (1, 4)
    # 数据转化成图像
    gps = np.reshape(np.array(np.fromiter(
        pointer_f_gps, dtype=np.float, count=size[0]*size[1])), size)
    print("数组模式获取的GPS:", gps)
    return gps
# -----------------2-1 获取gps数组结束----------------

# --------------------2-2 结构体拿GPS ----------------
# python创建结构体


class py_Struct_GPS(ctypes.Structure):
    _fields_ = [
        ('flag', ctypes.c_int),  # c++ int
        ('msg', ctypes.c_char_p),  # c++   char*
        ('longitude', ctypes.c_float),  # c++ float
        ('latitude', ctypes.c_float),  # c++ float
        ('high', ctypes.c_float),  # c++ float
        ('time', ctypes.c_float)  # c++ float
    ]  # c++ char*[]


# python结构体赋予初值
struct_gps = py_Struct_GPS()
struct_gps.flag = ctypes.c_int(0)
struct_gps.msg = ctypes.c_char_p('GPS DATA'.encode())
struct_gps.longitude = ctypes.c_float(0.)
struct_gps.latitude = ctypes.c_float(0.)
struct_gps.high = ctypes.c_float(0.)
struct_gps.time = ctypes.c_float(0.)

# print("结构体gps_old \n", struct_gps)
# print(struct_gps.flag, struct_gps.msg.decode(), struct_gps.longitude,
#       struct_gps.latitude, struct_gps.high, struct_gps.time)
# 0 GPS DATA 0.0 0.0 0.0 0.0

# 从c++获取数据-数据保存在 共享内存或C++直接生成
# 定义返回类型为结构体类型
sharelib.py_get_cgg_gps_Struct_.restype = py_Struct_GPS
# 调用获取gps数据-返回数据结构体
struct_gps = sharelib.py_get_cgg_gps_Struct_(struct_gps)
# print(struct_gps.flag)
# print(struct_gps.msg.decode())  #必须解码
# print("结构体gps_new \n", struct_gps)
# print(struct_gps.flag, struct_gps.msg.decode(), struct_gps.longitude,
#       struct_gps.latitude, struct_gps.high, struct_gps.time)
# 1 new data 1.559999942779541 2.559999942779541 3.559999942779541 5.559999942779541

# --------------------2-2 结构体拿GPS ----------------


# -----------------3python read cgg 读取图像-------------------

def ReadImgFromShare():
    start_time = time.time()
    # 设置输出数据类型为uint8的指针
    sharelib.Img_Cgg2py_.restype = ctypes.POINTER(ctypes.c_uint8)  # c_char_p
    # 获取输出图像数据指针
    pointer_img = sharelib.Img_Cgg2py_()
    size = (720, 1280, 3)
    # 数据转化成图像
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
        #展示图像
        #cv2.namedWindow("showimg",0)
        cv2.imshow("showimg ", RecImg)
        cv2.waitKey(1)
        #return RecImg
    else:
        return 1
    '''
    return RecImg


while True:

    time.sleep(0.001)  # 防止绝对的空循环导致CPu占用，其他进程无法修改共享内存

    if sharelib.Get_ImgFlag_() == 1:  # 存图结束，允许读图

        # 方法1-直接访问共享内存接受GPS数据
        # PyArr1x4Gps=ReadGPSFromShare()#获取GPS
        # print("python直接访问共享内存GPS数据索引:\n",PyArr1x4Gps[0][0],PyArr1x4Gps[0][1],PyArr1x4Gps[0][2],PyArr1x4Gps[0][3])

        # 方法2-通过结构体访问共享内存接受GPS数据
        # 调用获取gps数据-返回数据结构体
        # struct_gps= sharelib.py_get_cgg_gps_Struct_(struct_gps)
        # print("python通过结构体获取共享内存GPS数据：\n",struct_gps.flag,struct_gps.msg.decode(),struct_gps.longitude,struct_gps.latitude,struct_gps.high,struct_gps.time)

       # 接受图像
        Py_img = ReadImgFromShare()

        # 展示图像
        # cv2.namedWindow("showimg1",0)
        # cv2.imshow("showimg1 ", Py_img)

        # cv2.waitKey(1)

        sharelib.Set_ImgFalg_(0)  # 读图结束，允许存图

    if sharelib.Get_ImgFlag_() == 3:
        break

