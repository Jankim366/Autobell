'''
遇到问题没人解答？小编创建了一个Python学习交流QQ群：857662006 
寻找有志同道合的小伙伴，互帮互助,群里还有不错的视频学习教程和PDF电子书！
'''
# -*- coding=utf-8 -*-
import socket
from time import ctime
import json
import time
HOST = '192.168.43.107'
PORT = 8080
ADDR = (HOST, PORT)
BUFFSIZE = 4096
MAX_LISTEN = 10


def tcpServer():
    # TCP服务
    # with socket.socket() as s:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        # 绑定服务器地址和端口
        s.bind(ADDR)
        # 启动服务监听
        s.listen(MAX_LISTEN)
        print('等待用户接入。。。。。。。。。。。。')
        while True:
            # 等待客户端连接请求,获取connSock
            conn, addr = s.accept()
            print('警告，远端客户:{} 接入系统！！！'.format(addr))
            with conn:
                while True:
                    data_new = []
                    print('接收请求信息。。。。。')
                    # 接收请求信息
                    data = b'\x00RB001RB000POWER'
                    data = conn.recv(BUFFSIZE)
                    if data !=None:     #判断data是否为空，非空则有人，发送报文
                        # print("有人来了")
                        data=str(data, encoding="ISO-8859-1")
                        data=data.replace("\r\n","")
                        data=data[-23:-1]
                        data = data.encode('utf-8')
                        print('有人来了，接收报文data=%s' % data)
                        data = str(data)[23:-1].split('\\x')
                        for i in data:
                            data_new.append(int(i[:2], 16))
                        data = data_new
                        print('接收数据：{!r}'.format(data))
                    elif data == b'ATE':
                        continue
            s.close()




if __name__ == '__main__':

    while True:
        print('execute tcpsever')
        tcpServer()
