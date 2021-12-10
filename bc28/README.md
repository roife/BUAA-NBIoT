bsp：         存放BSP驱动文件（如LED，电机，温湿度传感器等）, 共用工具类文件（如MD5，字符串处理函数等）；

Drivers：     存放STM32平台的HAL库文件和CMSIS接口文件；

MDK-ARM：     存放Keil工程启动文件；

RTOS:         存放操作系统RTOS相关文件（如OS线程，日志等）；

system：      存放main函数入口文件和是STM32系统接口文件（如中断，系统时钟初始化、HAL初始化等）；

user：        存放用户线程文件（如 与通信模组的AT操作和业务实现函数等）；
