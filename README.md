# UE4_XunfeiSpeechReco
ue4 讯飞离线命令词识别插件

首先需要去讯飞开发者平台申请sdk，目前x64版本的sdk还有编译问题，需要单独找客服重新打包一个sdk。
替换插件目录中的.lib .dll文件
XunfeiReco.cpp中的appid修改为sdk中的id
call.bnf文件目前定义了几组词的识别（退出 10086等），复杂的用法参看官网

运行程序后，按R 开始录制 松手自动识别
