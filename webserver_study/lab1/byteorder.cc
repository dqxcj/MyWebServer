/*
 * @Author: ljy
 * @Date: 2023-04-28 15:41:40
 * @LastEditors: ljy
 * @LastEditTime: 2023-04-28 15:57:20
 * @FilePath: /MyWebServer/webserver_study/lab1/byteorder.cc
 * @Description: 测试主机字节序
 * Copyright (c) 2023 by ljy.sj@qq.com, All Rights Reserved. 
 */
#include <iostream>
using namespace std;

union ByteOrder {
    short value;
    char union_bytes[sizeof(short)];
}test;

int main() {
    test.value = 0x1234;
    if(test.union_bytes[0] == 0x34 && test.union_bytes[1] == 0x12) {
        cout << "little" << endl;
    } else if(test.union_bytes[0] == 0x12 && test.union_bytes[0] == 0x34) {
        cout << "big" << endl;
    } else {
        cout << "unknown..." << endl;
    }
    return 0;
}
