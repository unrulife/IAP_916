# 使用说明

## USB 透传

### 往OUT端点写入数据

1. 通过VID、PID、Interface index、EP OUT address确定要写入数据的端点

2. 在Data Send文本域中填写要写入的数据，默认以ascii方式解析，如果前缀是“0x”，则以16进制方式解析

   例如："Ingchips"，"0x31323334"

3. 指定写入数据的长度：WRITE SIZE

4. 点击Write按钮开始传输数据

### 从IN端点读取数据

1. 通过VID、PID、Interface index、EP OUT address确定要写入数据的端点

2. 指定读取数据的长度：Read SIZE

3. 点击Read按钮开始传输数据

4. 通过Data Recv文本域查看读取的数据，默认以16进制方式解析

错误信息会输出在底部的日志窗口

## USB设备树

可以查看设备的描述符信息