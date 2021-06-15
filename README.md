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
```

`$HOME/temp/` 文件夹下会生成一编码后的文件`*.lyra`

类似地，可以编译解码端并作用于encoder_main的输出上，以得到解码后的音频

```shell
bazel build -c opt :decoder_main
bazel-bin/decoder_main  --model_path=wavegru --output_dir=$HOME/temp/ --encoded_path=$HOME/temp/16khz_sample_000001.lyra
```

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
                "--model_path=wavegru",
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