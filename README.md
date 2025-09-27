# hinalang compiler

hinalangは「LLVMで作るプログラミング言語」で制作するプログラミング言語です。  
if-else文、while文が実装されています。

内部では、LLVM C++ APIを用いてLLVM IRを生成し、mem2regで最適化を行ってからオブジェクトコードを生成します。

## sample source
```
// prototype declare
fn puts(ptr c) -> i32;

fn main() -> i32 {
    puts("Hello, world!");
    return 0;
}
```

## how to compile
LLVM 21.1.6で動作確認しています。

```sh
$ git clone https://github.com/nikachu2012/hinalang.git
$ cd hinalang
$ make
$ ./a.out -o helloworld.o sample/helloworld.hina
$ clang -o helloworld helloworld.o
```
