######################################################################
##                                                                  
##   遍历指定目录获取当前目录下指定后缀（wav）的文件名          
##   对指定文件编码，部分文件目前无法编码
##                                                                  
######################################################################
 
##递归遍历
traverse_dir()
{
    filepath=$1
    
    for file in `ls -a $filepath`
    do
        if [ -d ${filepath}/$file ]
        then
            if [[ $file != '.' && $file != '..' ]]
            then
                #递归
                traverse_dir ${filepath}/$file
            fi
        else
            #调用查找指定后缀文件
            check_suffix ${filepath}/$file
        fi
    done
}
 
 
##获取后缀为txt或ini的文件
check_suffix()
{
    file=$1
    
    # if [ "${file##*.}"x = "wav"x ] || [ "${file##*.}"x = "txt"x ];then
    if [ "${file##*.}"x = "wav"x ];then
        echo $file
        bazel-bin/encoder_main --model_path=wavegru --output_dir=example/CHINESE_MANDARIN_3kbps/ --input_path=$file # 编码，注意修改文件夾
    fi    
}


#测试指定目录  /data_output/ci/history
# traverse_dir example/McGillTSP-16k-LP7-FA
traverse_dir example/CHINESE_MANDARIN_3kbps
