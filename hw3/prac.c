#include <iostream>
#include <stdio.h>

int main(int argc, char* argv[]) {
    std::cout << "程序名: " << argv[0] << std::endl;
    std::cout << "参数数量: " << argc - 1 << std::endl;
    
    for (int i = 1; i < argc; i++) {
        std::cout << "参数 " << i << ": " << argv[i] << std::endl;
    }
    int a = 1;
    std::cout << "a: " << a << std::endl;
    printf("a: %d \n",a);

    
    return 0;  // 返回 0 表示正常退出
}