# Lyra: a generative low bitrate speech codec

ref: [Lyra's README.md](https://github.com/google/lyra/blob/main/README.md)

## 配置环境

工作平台 ubuntu 20.04

### Common setup
```shell
sudo apt update
sudo apt install ninja-build git cmake clang python
```

### Linux requirements
```shell
git clone https://github.com/llvm/llvm-project.git # 不要克隆到lyra/文件夹下
cd llvm-project
git checkout 96ef4f307df2

mkdir build_clang # 依然是在llvm-project/ 文件夹下
cd build_clang

cmake -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DLLVM_ENABLE_PROJECTS="clang" -DCMAKE_BUILD_TYPE=release ../llvm

ninja # 耗时很久 虚拟机4小时

sudo $(which ninja) install

cd ..
mkdir build_libcxx
cd build_libcxx

cmake -G Ninja -DCMAKE_C_COMPILER=/usr/local/bin/clang -DCMAKE_CXX_COMPILER=/usr/local/bin/clang++ -DLLVM_ENABLE_PROJECTS="libcxx;libcxxabi" -DCMAKE_BUILD_TYPE=release ../llvm

ninja # 耗时很久 虚拟机 2小时

sudo $(which ninja) install

sudo ldconfig
```

### Building for Linux

Bazel verson 4.0.0 is required

[安装Bazel](https://docs.bazel.build/versions/master/install.html)

使用apt安装可能会有问题（无法定位bazel），推荐[使用二进制安装方式](https://docs.bazel.build/versions/main/install-ubuntu.html#install-with-installer-ubuntu)

#### ubuntu 20.04 安装 bazel 
```shell
sudo apt install g++ unzip zip

# If you want to build Java code using Bazel, install a JDK:
# Ubuntu 18.04 (LTS) uses OpenJDK 11 by default:
sudo apt-get install openjdk-11-jdk
```

下载 `bazel-<version>-installer-linux-x86_64.sh`, [Bazel releases page on GitHub](https://github.com/bazelbuild/bazel/releases) 下载 4.0.0版本的

```shell
chmod +x bazel-4.0.0-installer-linux-x86_64.sh
./bazel-4.0.0-installer-linux-x86_64.sh --user
```

添加到环境变量
```shell
export PATH="$PATH:$HOME/bin"
```

设置git的翻墙代理(假设本机已经可以翻墙, 且配置好了系统代理)，不然bazel过程中的github clone 会出问题
```shell
git config --global http.proxy 'socks5://127.0.0.1:1088'
git config --global https.proxy 'socks5://127.0.0.1:1088'
```

用完代理别忘了取消
```shell
git config --global --unset http.proxy
git config --global --unset https.proxy
```

或者配置linux终端走代理的方式
```shell
export http_proxy="socks5://127.0.0.1:1088"
export https_proxy="socks5://127.0.0.1:1088"
```
由于代理不稳定，别忘了取消代理
```shell
unset http_proxy
unset https_proxy
```

使用默认配置build cc_binaries，进入 `lyra/` 根目录，输入以下命令
```shell
bazel build -c opt :encoder_main
```

您可以运行`encoder_main`对`test.wav`文件进行编码，对应`--input_path`. `--model_path`表示包含编码所需的模型数据，`--output_path`指定在何处写入编码后的（压缩的）表示。

```shell
bazel-bin/encoder_main --model_path=wavegru --output_dir=$HOME/temp --input_path=testdata/16khz_sample_000001.wav

# output
WARNING: Logging before InitGoogleLogging() is written to STDERR
I20210616 09:27:20.503815  9142 encoder_main_lib.cc:94] Elapsed seconds : 0 # 耗时0秒
I20210616 09:27:20.504074  9142 encoder_main_lib.cc:95] Samples per second : 3.72564e+06
```

`$HOME/temp/` 文件夹下会生成一编码后的文件`*.lyra`

类似地，可以编译解码端并作用于encoder_main的输出上，以得到解码后的音频

```shell
bazel build -c opt :decoder_main
bazel-bin/decoder_main --model_path=wavegru --output_dir=$HOME/temp/ --encoded_path=$HOME/temp/16khz_sample_000001.lyra

# output
WARNING: Logging before InitGoogleLogging() is written to STDERR
W20210616 09:31:21.751451  9802 lyra_wavegru.h:80] lyra_wavegru running in slow generic mode.
I20210616 09:31:21.753964  9802 layer_wrapper.h:96] |lyra_16khz_ar_to_gates_| layer:  Shape: [3072, 4]. Sparsity: 0
I20210616 09:31:22.032354  9802 layer_wrapper.h:96] |lyra_16khz_gru_layer_| layer:  Shape: [3072, 1024]. Sparsity: 0.9375
I20210616 09:31:22.070669  9802 lyra_wavegru.h:226] Model size: 1271266 bytes
I20210616 09:31:22.070955  9802 wavegru_model_impl.cc:87] Feature size: 160
I20210616 09:31:22.071512  9802 wavegru_model_impl.cc:88] Number of samples per hop: 640
I20210616 09:31:22.089547  9802 layer_wrapper.h:96] |lyra_16khz_conv1d_| layer:  Shape: [512, 480]. Sparsity: 0.919987
I20210616 09:31:22.126284  9802 layer_wrapper.h:96] |lyra_16khz_conditioning_stack_0_| layer:  Shape: [512, 1024]. Sparsity: 0.920013
I20210616 09:31:22.172271  9802 layer_wrapper.h:96] |lyra_16khz_conditioning_stack_1_| layer:  Shape: [512, 1024]. Sparsity: 0.920013
I20210616 09:31:22.220367  9802 layer_wrapper.h:96] |lyra_16khz_conditioning_stack_2_| layer:  Shape: [512, 1024]. Sparsity: 0.920013
I20210616 09:31:22.261426  9802 layer_wrapper.h:96] |lyra_16khz_transpose_0_| layer:  Shape: [1024, 512]. Sparsity: 0.920013
I20210616 09:31:22.295868  9802 layer_wrapper.h:96] |lyra_16khz_transpose_1_| layer:  Shape: [1024, 512]. Sparsity: 0.920013
I20210616 09:31:22.337258  9802 layer_wrapper.h:96] |lyra_16khz_transpose_2_| layer:  Shape: [1024, 512]. Sparsity: 0.920013
I20210616 09:31:22.374413  9802 layer_wrapper.h:96] |lyra_16khz_conv_cond_| layer:  Shape: [1024, 512]. Sparsity: 0.920013
I20210616 09:31:22.586452  9802 layer_wrapper.h:96] |lyra_16khz_conv_to_gates_| layer:  Shape: [3072, 1024]. Sparsity: 0.919998
WARNING: Logging before InitGoogleLogging() is written to STDERR
W20210616 09:31:22.599963  9802 kernels_generic.h:241] SumVectors: using generic kernel!
I20210616 09:31:34.903664  9802 decoder_main_lib.cc:96] Elapsed seconds : 12 # 耗时 12秒
I20210616 09:31:34.903755  9802 decoder_main_lib.cc:97] Samples per second : 9827.37
```

## debug with vscode

如果要进行debug，使用vscode 进行debug

encoder_main的debug配置， decoder_main的debug配置同理，debug只有第一次需要链接全部文件，所以耗时较久，后面就好了

launch.json 文件 
```shell
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Bazel build",
            "preLaunchTask": "build",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/bazel-bin/encoder_main",
            "args": [
                "--model_path=${workspaceFolder}/wavegru",
                "--output_dir=$HOME/temp",
                "--input_path=${workspaceFolder}/testdata/16khz_sample_000001.wav"
            ],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}/bazel-bin/encoder_main.runfiles/__main__/",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "sourceFileMap": {"/proc/self/cwd":"${workspaceFolder}"}, // https://gitter.im/vscode-lldb/QnA?at=5e30bb93594a0517c2407b11
            "miDebuggerArgs": "-q -ex quit; wait() { fg >/dev/null; }; /bin/gdb -q --interpreter=mi", // https://github.com/microsoft/vscode-cpptools/issues/3298
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": false
                }
            ]
        },
    ]
}
```

tasks.json 文件
```shell
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            "command": "bazel build -c dbg :encoder_main",
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}
```


### Android requirement
[下载安卓 sdkmanager](https://developer.android.com/studio#command-tools)

解压 并 cd 到解压目录, 检查要安装的可用软件包，以防它们与以下步骤不匹配。
```shell
bin/sdkmanager  --sdk_root=$HOME/android/sdk --list
```
Some systems will already have the java runtime set up.  But if you see an error
here like `ERROR: JAVA_HOME is not set and no 'java' command could be found
on your PATH.`, this means you need to install the java runtime with `sudo apt
install default-jdk` first. You will also need to add `export
JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64` (type `ls /usr/lib/jvm` to see
which path was installed) to your $HOME/.bashrc and reload it with `source
$HOME/.bashrc`.

Install the r21 ndk, android sdk 29, and build tools:
```shell
bin/sdkmanager  --sdk_root=$HOME/android/sdk --install  "platforms;android-29" "build-tools;29.0.3" "ndk;21.4.7075529"
```

添加环境变量到`.bashrc`
```shell
export ANDROID_NDK_HOME=$HOME/android/sdk/ndk/21.4.7075529
export ANDROID_HOME=$HOME/android/sdk
```

重载环境变量 
```shell
source $HOME/.bashrc
```


### build for andorid

此示例是一个具有最小 GUI 的应用程序，其中包含两个选项的按钮。 一种选择是从麦克风录音并使用 Lyra 进行编码/解码，以便您 可以测试 Lyra 听起来像你的声音。 另一个选项运行一个 在后台编码和解码并将时间打印到的基准测试 logcat。
```shell
sudo apt install adb # 后面会用
```

命令行进入到 `lyra/` 目录下

 ```shell
bazel build android_example:lyra_android_example --config=android_arm64 --copt=-DBENCHMARK
```
编译完之后，得到`lyra_android_example.apk`

安卓机需要打开开发者模式，然后安装到手机,配置开发者选项(小米8)：
设置->我的设备->全部参数->MIUI版本(点5次)->退回到更多设置->开发者选项->usb调试

```shell
lsusb # 查看挂载的usb
adb devices [-l] # 查看挂载的设备 -l 是输出更多信息

# 实例
lisen@lisen-ubuntu:~/workspace/code/lyra$ lsusb
Bus 001 Device 007: ID 18d1:4ee7 Google Inc. MI 8
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 002 Device 003: ID 0e0f:0002 VMware, Inc. Virtual USB Hub
Bus 002 Device 002: ID 0e0f:0003 VMware, Inc. Virtual Mouse
Bus 002 Device 001: ID 1d6b:0001 Linux Foundation 1.1 root hub
lisen@lisen-ubuntu:~/workspace/code/lyra$ adb devices -l
List of devices attached
a0737173               device usb:1-1 product:dipper model:MI_8 device:dipper transport_id:2
```

安装到手机，别忘了通过手机应用安装提示
```
adb install bazel-bin/android_example/lyra_android_example.apk
```
