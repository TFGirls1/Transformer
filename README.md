# Transformer
## Tokenization
考虑到计算机没有办法直接识别人类语言，我们将每一个词映射为一个token使得计算机可以直接识别。
为实现这个目的我们使用BPE算法将每个词划分为若干个前缀和后缀，以此拼起来一个词，节省vocabulary占用的空间。
这一部分写于BPE.h
