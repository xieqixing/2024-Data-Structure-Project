# 子目录下的cmakelists.txt文件 
#
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

# 生成(静态)链接库
add_library (tinyXml ${DIR_LIB_SRCS})

