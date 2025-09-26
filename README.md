# hinalang compiler

hinalangは「LLVMで作るプログラミング言語」で制作するプログラミング言語です。  
if-else文、while文が実装されています。

内部では、LLVM C++ APIを用いてLLVM IRを生成し、mem2regで最適化を行ってからオブジェクトコードを生成します。

## sample source
```
fn puts(ptr p) -> i32;

fn main() -> i64 {
    puts("Hello world!");
    return 0;
}
```

## how to compile
LLVM 20.1.6で動作確認しています。

```sh
$ make
$ ./a.out input -o output
```
